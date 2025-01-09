pub struct BinaryString(pub String);
impl BinaryString {
    pub fn from_string_u16(value: String, width: usize) -> String {
        BinaryString(format!("{:0>width$b}", value.parse::<u16>().unwrap_or_else(|e| panic!("Error from_u32 {:?} - {:?}", value, e)), width = width)).0
    }
    pub fn format_string_10_bytes(value: u16) -> String {
        format!("{:0>10b}", value)
    }
}