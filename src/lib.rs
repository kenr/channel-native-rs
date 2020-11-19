#![allow(non_camel_case_types, non_upper_case_globals, non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

unsafe extern "C" fn callback_trampoline<F>(
    message: *const ::std::os::raw::c_char,
    user_data: *const ::std::os::raw::c_void,
) where
    F: FnMut(String),
{
    let user_data = &mut *(user_data as *mut F);

    let _message = std::ffi::CStr::from_ptr(message)
        .to_string_lossy()
        .into_owned();

    user_data(_message);
}

fn get_callback_trampoline<F>(_closure: &F) -> callback_t
where
    F: FnMut(String),
{
    Some(callback_trampoline::<F>)
}

pub fn register_callback_safe<C>(mut c: C)
where
    C: Fn(String) + Send + Sync,
{
    let _callback_trampoline = get_callback_trampoline(&c);

    unsafe {
        register_callback(
            _callback_trampoline,
            &mut c as *mut _ as *mut ::std::os::raw::c_void,
        );
    }
}

pub fn start_safe(number_of_threads: u32) {
    unsafe { start(number_of_threads) }
}

pub fn stop_safe() {
    unsafe { stop() }
}
