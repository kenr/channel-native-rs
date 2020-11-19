use crossbeam_channel::unbounded;

fn main() {
    let (sender, receiver) = unbounded();

    let callback = |message: String| {
        // println!("{}", message);
        sender.send(message).unwrap(); // Memory access violation here
    };

    let number_of_threads = 100;

    // Register a callback that will be called from different threads
    // by the native library
    lib::register_callback_safe(callback);
    println!("Starting {} threads", number_of_threads);
    lib::start_safe(number_of_threads);

    // Just pick up one of the elements in the channel
    println!("Received: {:?}", receiver.recv().unwrap());

    println!("Waiting for threads to join");
    lib::stop_safe();
    println!("All {} threads joined", number_of_threads);
}
