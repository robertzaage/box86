/*****************************************************************
 * File automatically generated by rebuild_wrappers.py (v1.2.0.09)
 *****************************************************************/
#ifndef __wrappedgio2TYPES_H_
#define __wrappedgio2TYPES_H_

#ifndef LIBNAME
#error You should only #include this file inside a wrapped*.c file
#endif
#ifndef ADDED_FUNCTIONS
#define ADDED_FUNCTIONS() 
#endif

typedef void (*vFppp_t)(void*, void*, void*);
typedef void (*vFippp_t)(int32_t, void*, void*, void*);
typedef void (*vFppip_t)(void*, void*, int32_t, void*);
typedef void (*vFpppp_t)(void*, void*, void*, void*);
typedef uint32_t (*uFpppp_t)(void*, void*, void*, void*);
typedef uintptr_t (*LFpppp_t)(void*, void*, void*, void*);
typedef void* (*pFpppp_t)(void*, void*, void*, void*);
typedef void (*vFpippp_t)(void*, int32_t, void*, void*, void*);
typedef void (*vFpipppp_t)(void*, int32_t, void*, void*, void*, void*);
typedef void (*vFiippppV_t)(int32_t, int32_t, void*, void*, void*, void*, void*);
typedef void (*vFiupippp_t)(int32_t, uint32_t, void*, int32_t, void*, void*, void*);
typedef void (*vFippippp_t)(int32_t, void*, void*, int32_t, void*, void*, void*);
typedef void (*vFppipppp_t)(void*, void*, int32_t, void*, void*, void*, void*);
typedef void (*vFpppuipV_t)(void*, void*, void*, uint32_t, int32_t, void*, void*);
typedef uint32_t (*uFipipppp_t)(int32_t, void*, int32_t, void*, void*, void*, void*);
typedef uint32_t (*uFppipppp_t)(void*, void*, int32_t, void*, void*, void*, void*);
typedef uint32_t (*uFppppppp_t)(void*, void*, void*, void*, void*, void*, void*);
typedef void* (*pFpppuipV_t)(void*, void*, void*, uint32_t, int32_t, void*, void*);
typedef void (*vFppiipppp_t)(void*, void*, int32_t, int32_t, void*, void*, void*, void*);
typedef void (*vFpppiippp_t)(void*, void*, void*, int32_t, int32_t, void*, void*, void*);
typedef uint32_t (*uFipippppp_t)(int32_t, void*, int32_t, void*, void*, void*, void*, void*);
typedef void (*vFiippppppp_t)(int32_t, int32_t, void*, void*, void*, void*, void*, void*, void*);
typedef void (*vFpippppppp_t)(void*, int32_t, void*, void*, void*, void*, void*, void*, void*);
typedef void (*vFpppiipppp_t)(void*, void*, void*, int32_t, int32_t, void*, void*, void*, void*);
typedef void* (*pFiippppppp_t)(int32_t, int32_t, void*, void*, void*, void*, void*, void*, void*);
typedef uint32_t (*uFppppppippp_t)(void*, void*, void*, void*, void*, void*, int32_t, void*, void*, void*);
typedef void (*vFpppppppiippp_t)(void*, void*, void*, void*, void*, void*, void*, int32_t, int32_t, void*, void*, void*);

#define SUPER() ADDED_FUNCTIONS() \
	GO(g_simple_async_result_set_op_res_gpointer, vFppp_t) \
	GO(g_bus_get, vFippp_t) \
	GO(g_simple_async_result_run_in_thread, vFppip_t) \
	GO(g_dbus_connection_close, vFpppp_t) \
	GO(g_dbus_connection_flush, vFpppp_t) \
	GO(g_simple_async_report_gerror_in_idle, vFpppp_t) \
	GO(g_simple_async_report_take_gerror_in_idle, vFpppp_t) \
	GO(g_dbus_connection_add_filter, uFpppp_t) \
	GO(g_cancellable_connect, LFpppp_t) \
	GO(g_simple_async_result_new, pFpppp_t) \
	GO(g_simple_async_result_new_from_error, pFpppp_t) \
	GO(g_simple_async_result_new_take_error, pFpppp_t) \
	GO(g_async_initable_init_async, vFpippp_t) \
	GO(g_dbus_connection_new_for_address, vFpipppp_t) \
	GO(g_async_initable_new_async, vFiippppV_t) \
	GO(g_async_initable_newv_async, vFiupippp_t) \
	GO(g_async_initable_new_valist_async, vFippippp_t) \
	GO(g_dbus_connection_new, vFppipppp_t) \
	GO(g_simple_async_report_error_in_idle, vFpppuipV_t) \
	GO(g_bus_watch_name, uFipipppp_t) \
	GO(g_bus_own_name_on_connection, uFppipppp_t) \
	GO(g_bus_watch_name_on_connection, uFppipppp_t) \
	GO(g_dbus_connection_register_object, uFppppppp_t) \
	GO(g_simple_async_result_new_error, pFpppuipV_t) \
	GO(g_dbus_connection_send_message_with_reply, vFppiipppp_t) \
	GO(g_dbus_proxy_call, vFpppiippp_t) \
	GO(g_bus_own_name, uFipippppp_t) \
	GO(g_dbus_proxy_new_for_bus, vFiippppppp_t) \
	GO(g_dbus_proxy_new, vFpippppppp_t) \
	GO(g_dbus_proxy_call_with_unix_fd_list, vFpppiipppp_t) \
	GO(g_dbus_object_manager_client_new_for_bus_sync, pFiippppppp_t) \
	GO(g_dbus_connection_signal_subscribe, uFppppppippp_t) \
	GO(g_dbus_connection_call, vFpppppppiippp_t)

#endif // __wrappedgio2TYPES_H_