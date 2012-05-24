/*
 * gawkapi.c -- Implement the functions defined for gawkapi.h
 */

/* 
 * Copyright (C) 2012, the Free Software Foundation, Inc.
 * 
 * This file is part of GAWK, the GNU implementation of the
 * AWK Programming Language.
 * 
 * GAWK is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GAWK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "awk.h"

static awk_value_t *node_to_awk_value(NODE *node, awk_value_t *result);

/*
 * Get the count'th paramater, zero-based.
 * Returns NULL if count is out of range, or if actual paramater
 * does not match what is specified in wanted.
 */
static awk_value_t *
api_get_curfunc_param(awk_ext_id_t id, size_t count,
			awk_valtype_t wanted, awk_value_t *result)
{
	NODE *arg;

	arg = (wanted == AWK_ARRAY
			? get_array_argument(count, false)
			: get_scalar_argument(count, false) );
	if (arg == NULL)
		return NULL;

	if (arg->type != Node_var_array) {
		if (wanted == AWK_NUMBER) {
			(void) force_number(arg);
		} else {
			(void) force_string(arg);
		}
	}

	return node_to_awk_value(arg, result);
}

/* Set the return value. Gawk takes ownership of string memory */
NODE *
awk_value_to_node(const awk_value_t *retval)
{
	NODE *ext_ret_val;

	if (retval == NULL)
		fatal(_("api_set_return_value: received null retval"));

	ext_ret_val = NULL;
	getnode(ext_ret_val);
	if (retval->val_type == AWK_ARRAY) {
		*ext_ret_val = *((NODE *) retval->array_cookie);
	} else if (retval->val_type == AWK_UNDEFINED) {
		/* free node */
		ext_ret_val = dupnode(Nnull_string);
	} else if (retval->val_type == AWK_NUMBER) {
		ext_ret_val = make_number(retval->num_value);
	} else {
		ext_ret_val = make_string(retval->str_value.str, retval->str_value.len);
	}
	return ext_ret_val;
}

/* Functions to print messages */
/* FIXME: Code duplicate from msg.c. Fix this. */
static void
api_fatal(awk_ext_id_t id, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	err(_("fatal: "), format, args);
	va_end(args);
#ifdef GAWKDEBUG
	abort();
#endif
	gawk_exit(EXIT_FATAL);
}

static void
api_warning(awk_ext_id_t id, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	err(_("warning: "), format, args);
	va_end(args);
}

static void
api_lintwarn(awk_ext_id_t id, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (lintwarn == r_fatal) {
		err(_("fatal: "), format, args);
		va_end(args);
#ifdef GAWKDEBUG
		abort();
#endif
		gawk_exit(EXIT_FATAL);
	} else {
		err(_("warning: "), format, args);
		va_end(args);
	}
}

/* Register an open hook; for opening files read-only */

static void
api_register_open_hook(awk_ext_id_t id, void* (*open_func)(IOBUF *))
{
	register_open_hook(open_func);
}

/* Functions to update ERRNO */
static void
api_update_ERRNO_int(awk_ext_id_t id, int errno_val)
{
	update_ERRNO_int(errno_val);
}

static void
api_update_ERRNO_string(awk_ext_id_t id, const char *string,
		awk_bool_t translate)
{
	update_ERRNO_string(string, translate);
}

static void
api_unset_ERRNO(awk_ext_id_t id)
{
	unset_ERRNO();
}


/* Add a function to the interpreter, returns true upon success */
static awk_bool_t
api_add_ext_func(awk_ext_id_t id,
		const awk_ext_func_t *func,
		const char *namespace)
{
	return make_builtin(func);
}

/* Stuff for exit handler - do it as linked list */

struct ext_exit_handler {
	struct ext_exit_handler *next;
	void (*funcp)(void *data, int exit_status);
	void *arg0;
};
static struct ext_exit_handler *list_head = NULL;

/* run_ext_exit_handlers --- run the extension exit handlers, LIFO order */

void
run_ext_exit_handlers(int exitval)
{
	struct ext_exit_handler *p, *next;

	for (p = list_head; p != NULL; p = next) {
		next = p->next;
		p->funcp(p->arg0, exitval);
		free(p);
	}
	list_head = NULL;
}

/* Add an exit call back, returns true upon success */
static awk_bool_t
api_awk_atexit(awk_ext_id_t id,
		void (*funcp)(void *data, int exit_status),
		void *arg0)
{
	struct ext_exit_handler *p;

	/* allocate memory */
	emalloc(p, struct ext_exit_handler *, sizeof(struct ext_exit_handler), "api_awk_atexit");

	/* fill it in */
	p->funcp = funcp;
	p->arg0 = arg0;

	/* add to linked list, LIFO order */
	p->next = list_head;
	list_head = p;
	return true;	/* for now */
}

/*
 * Symbol table access:
 * 	- No access to special variables (NF, etc.)
 * 	- One special exception: PROCINFO.
 *	- Use sym_update() to change a value, including from UNDEFINED
 *	  to scalar or array. 
 */
static awk_value_t *
node_to_awk_value(NODE *node, awk_value_t *val)
{
	memset(val, 0, sizeof(*val));
	switch (node->type) {
	case Node_val:
		if (node->flags & NUMBER) {
			val->val_type = AWK_NUMBER;
			val->num_value = get_number_d(node);
		} else if (node->flags & STRING) {
			val->val_type = AWK_STRING;
			val->str_value.str = node->stptr;
			val->str_value.len = node->stlen;
		} else {
			return NULL;
		}

	case Node_var_new:
		val->val_type = AWK_UNDEFINED;
		break;
	case Node_var:
		if ((node->var_value->flags & NUMBER) != 0) {
			val->val_type = AWK_NUMBER;
			val->num_value = get_number_d(node->var_value);
		} else {
			val->val_type = AWK_STRING;
			val->str_value.str = node->var_value->stptr;
			val->str_value.len = node->var_value->stlen;
		}
		break;
	case Node_var_array:
		val->val_type = AWK_ARRAY;
		val->array_cookie = node;
		break;
	default:
		return NULL;
	}

	return val;
}

/*
 * Lookup a variable, return its value. No messing with the value
 * returned. Return value is NULL if the variable doesn't exist.
 */
static awk_value_t *
api_sym_lookup(awk_ext_id_t id, const char *name, awk_value_t *result)
{
	NODE *node;

	if (name == NULL || (node = lookup(name)) == NULL)
		return NULL;

	return node_to_awk_value(node, result);
}

/*
 * Update a value. Adds it to the symbol table if not there.
 * Changing types is not allowed.
 */
static awk_bool_t
api_sym_update(awk_ext_id_t id, const char *name, awk_value_t *value)
{
	return true;	/* for now */
}

/* Array management */
/*
 * Return the value of an element - read only!
 * Use set_array_element to change it.
 */
static awk_value_t *
api_get_array_element(awk_ext_id_t id,
		awk_array_t a_cookie, const awk_value_t *const index,
		awk_value_t *result)
{
	return NULL;	/* for now */
}

/*
 * Change (or create) element in existing array with
 * element->index and element->value.
 */
static awk_bool_t
api_set_array_element(awk_ext_id_t id, awk_array_t a_cookie,
				awk_element_t *element)
{
	NODE *array = (NODE *)a_cookie;
	NODE *tmp;
	NODE **aptr;

	tmp = make_string(element->index.str, element->index.len);
	aptr = assoc_lookup(array, tmp);
	unref(tmp);
	unref(*aptr);
	*aptr = awk_value_to_node(& element->value);
	return true;
}

/*
 * Remove the element with the given index.
 * Returns success if removed or if element did not exist.
 */
static awk_bool_t
api_del_array_element(awk_ext_id_t id,
		awk_array_t a_cookie, const awk_value_t* const index)
{
	return true;	/* for now */
}

/*
 * Retrieve total number of elements in array.
 * Returns false if some kind of error.
 */
static awk_bool_t
api_get_element_count(awk_ext_id_t id,
		awk_array_t a_cookie, size_t *count)
{
	NODE *node = (NODE *) a_cookie;

	if (node == NULL || node->type != Node_var_array)
		return false;

	*count = node->array_size;
	return true;
}

/* Create a new array cookie to which elements may be added */
static awk_array_t
api_create_array(awk_ext_id_t id)
{
	return NULL;	/* for now */
}

/* Clear out an array */
static awk_bool_t
api_clear_array(awk_ext_id_t id, awk_array_t a_cookie)
{
	NODE *node = (NODE *) a_cookie;

	if (node == NULL || node->type != Node_var_array)
		return false;

	assoc_clear(node);
	return true;
}

/* Flatten out an array so that it can be looped over easily. */
static awk_bool_t
api_flatten_array(awk_ext_id_t id,
		awk_array_t a_cookie,
		size_t *count,
		awk_element_t **data)
{
	return true;	/* for now */
}

/*
 * When done, release the memory, delete any marked elements
 * Count must match what gawk thinks the size is.
 */
static awk_bool_t
api_release_flattened_array(awk_ext_id_t id,
		awk_array_t a_cookie,
		size_t count,
		awk_element_t *data)
{
	return true;	/* for now */
}

gawk_api_t api_impl = {
	GAWK_API_MAJOR_VERSION,	/* major and minor versions */
	GAWK_API_MINOR_VERSION,
	{ 0 },			/* do_flags */

	api_get_curfunc_param,

	api_fatal,
	api_warning,
	api_lintwarn,

	api_register_open_hook,

	api_update_ERRNO_int,
	api_update_ERRNO_string,
	api_unset_ERRNO,

	api_add_ext_func,

	api_awk_atexit,

	api_sym_lookup,
	api_sym_update,

	api_get_array_element,
	api_set_array_element,
	api_del_array_element,
	api_get_element_count,
	api_create_array,
	api_clear_array,
	api_flatten_array,
	api_release_flattened_array,
};

/* init_ext_api --- init the extension API */

void
init_ext_api()
{
	api_impl.do_flags[0] = do_lint;
	api_impl.do_flags[1] = do_traditional;
	api_impl.do_flags[2] = do_profile;
	api_impl.do_flags[3] = do_sandbox;
	api_impl.do_flags[4] = do_debug;
	api_impl.do_flags[5] = do_mpfr;
}
