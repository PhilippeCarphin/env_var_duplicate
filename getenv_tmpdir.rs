
fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("TMPDIR={}", std::env::var("TMPDIR")?);
    Ok(())
}
