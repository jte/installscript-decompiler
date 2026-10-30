//! File-type detection, script decryption and the `.obl` library container.

use crate::Result;
use crate::stream::Stream;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum HeaderKind {
    Unrecognized,
    /// Compiled script object (`.obs`).
    Obs,
    /// Compiled setup script (`setup.inx`), stored encrypted.
    ALuZ,
    /// Compiled setup script with debug info, stored encrypted.
    KUtZ,
    /// Script library (`.obl`), a container of `.obs` files.
    Obl,
    /// InstallShield 5 and earlier script (`setup.ins`).
    Ins,
}

impl HeaderKind {
    pub fn description(self) -> &'static str {
        match self {
            HeaderKind::Unrecognized => "unrecognized",
            HeaderKind::Obs => "compiled script object (.obs)",
            HeaderKind::ALuZ => "compiled setup script (.inx)",
            HeaderKind::KUtZ => "compiled setup script with debug info (.inx)",
            HeaderKind::Obl => "script library (.obl)",
            HeaderKind::Ins => "InstallShield 5 script (.ins)",
        }
    }
}

fn magic(contents: &[u8]) -> Option<[u8; 4]> {
    contents.get(..4).map(|m| m.try_into().unwrap())
}

/// Decrypts `.inx` scripts in place. `seed` carries over between consecutive
/// chunks of the same stream.
pub fn decrypt(seed: &mut u32, buffer: &mut [u8], key: u8) {
    for (i, b) in buffer.iter_mut().enumerate() {
        let rotated = (*b ^ key).rotate_right(2);
        *b = rotated.wrapping_sub(((i as u64 + *seed as u64) % 0x47) as u8);
    }
    *seed = seed.wrapping_add(buffer.len() as u32);
}

/// Identifies `contents`. Encrypted scripts are decrypted in place, so
/// whenever the result is `ALuZ` or `KUtZ` the buffer holds plaintext.
pub fn header_kind(contents: &mut [u8]) -> HeaderKind {
    match magic(contents) {
        None => return HeaderKind::Unrecognized,
        Some([0x48, 0x4f, 0xf3, 0xc9]) => return HeaderKind::Obs,
        Some(m) if &m == b"pOdA" => return HeaderKind::Obl,
        Some(m) if &m == b"aLuZ" => return HeaderKind::ALuZ,
        Some([0xb8, 0xc9, 0x0c, 0x00]) => return HeaderKind::Ins,
        Some(_) => {}
    }
    let mut seed = 0;
    decrypt(&mut seed, contents, 0xf1);
    match magic(contents) {
        Some(m) if &m == b"aLuZ" => HeaderKind::ALuZ,
        Some(m) if &m == b"kUtZ" => HeaderKind::KUtZ,
        _ => HeaderKind::Unrecognized,
    }
}

/// Header detection without touching the input.
pub fn detect(contents: &[u8]) -> HeaderKind {
    let head = &contents[..contents.len().min(4)];
    header_kind(&mut head.to_vec())
}

pub struct LibScript {
    pub name: String,
    pub data: Vec<u8>,
}

/// Splits a `.obl` library into its member scripts.
pub fn parse_library(contents: &[u8]) -> Result<Vec<LibScript>> {
    let mut s = Stream::new(contents);
    if s.bytes(4)? != b"pOdA" {
        return Err("Not a script library (bad signature).".into());
    }
    let version = s.u32()?;
    if version != 1 {
        return Err(format!("Unsupported script library version {version}."));
    }
    let count = s.u32()?;
    let mut entries = Vec::new();
    for _ in 0..count {
        let name = s.ins_string()?;
        let offset = s.u32()? as usize;
        let length = s.u32()? as usize;
        entries.push((name, offset, length));
    }
    entries
        .into_iter()
        .map(|(name, offset, length)| {
            let data = Stream::at(contents, offset).bytes(length)?.to_vec();
            Ok(LibScript { name, data })
        })
        .collect()
}

#[cfg(test)]
pub fn encrypt(buffer: &mut [u8], key: u8) {
    for (i, b) in buffer.iter_mut().enumerate() {
        *b = b.wrapping_add((i % 0x47) as u8).rotate_left(2) ^ key;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn decrypt_inverts_encrypt() {
        let plain: Vec<u8> = b"aLuZ"
            .iter()
            .copied()
            .chain((0..=255u8).cycle().take(1000))
            .collect();
        let mut buf = plain.clone();
        encrypt(&mut buf, 0xf1);
        assert_ne!(buf, plain);
        assert_eq!(header_kind(&mut buf), HeaderKind::ALuZ);
        assert_eq!(buf, plain);
    }

    #[test]
    fn short_input_is_unrecognized() {
        assert_eq!(header_kind(&mut [1, 2]), HeaderKind::Unrecognized);
    }
}
