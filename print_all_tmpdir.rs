
fn main() {
    for (key, value) in std::env::vars() {
        if key == "TMPDIR" {
            println!("{key}: {value}");
        }
    }
}
