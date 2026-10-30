//! Little-endian cursor over a byte buffer. Every read is bounds-checked so
//! malformed input turns into an error instead of a panic.

use crate::Result;

#[derive(Clone)]
pub struct Stream<'a> {
    data: &'a [u8],
    pos: usize,
}

impl<'a> Stream<'a> {
    pub fn new(data: &'a [u8]) -> Self {
        Stream { data, pos: 0 }
    }

    pub fn at(data: &'a [u8], pos: usize) -> Self {
        Stream { data, pos }
    }

    /// A second cursor over the same buffer.
    pub fn fork_at(&self, pos: usize) -> Stream<'a> {
        Stream {
            data: self.data,
            pos,
        }
    }

    pub fn pos(&self) -> usize {
        self.pos
    }

    pub fn jump_to(&mut self, pos: usize) {
        self.pos = pos;
    }

    pub fn bytes(&mut self, n: usize) -> Result<&'a [u8]> {
        let end = self.pos.checked_add(n).filter(|&e| e <= self.data.len());
        match end {
            Some(end) => {
                let s = &self.data[self.pos..end];
                self.pos = end;
                Ok(s)
            }
            None => Err(format!(
                "Tried to read beyond end of file (offset {:#x}, {n} bytes).",
                self.pos
            )),
        }
    }

    fn array<const N: usize>(&mut self) -> Result<[u8; N]> {
        Ok(self.bytes(N)?.try_into().unwrap())
    }

    pub fn u8(&mut self) -> Result<u8> {
        Ok(self.array::<1>()?[0])
    }

    pub fn i8(&mut self) -> Result<i8> {
        Ok(self.u8()? as i8)
    }

    pub fn u16(&mut self) -> Result<u16> {
        Ok(u16::from_le_bytes(self.array()?))
    }

    pub fn i16(&mut self) -> Result<i16> {
        Ok(i16::from_le_bytes(self.array()?))
    }

    pub fn u32(&mut self) -> Result<u32> {
        Ok(u32::from_le_bytes(self.array()?))
    }

    pub fn i32(&mut self) -> Result<i32> {
        Ok(i32::from_le_bytes(self.array()?))
    }

    pub fn peek_u8(&self) -> Result<u8> {
        self.clone().u8()
    }

    pub fn peek_u16(&self) -> Result<u16> {
        self.clone().u16()
    }

    /// A string stored as a u16 length followed by that many ANSI bytes.
    pub fn ins_string(&mut self) -> Result<String> {
        let len = self.u16()? as usize;
        Ok(decode_ansi(self.bytes(len)?))
    }
}

/// InstallShield stores strings in the build machine's ANSI code page; decode
/// as Windows-1252, the most common one, so every byte maps to a character.
pub fn decode_ansi(bytes: &[u8]) -> String {
    const HIGH: [char; 32] = [
        '€', '\u{81}', '‚', 'ƒ', '„', '…', '†', '‡', 'ˆ', '‰', 'Š', '‹', 'Œ', '\u{8d}', 'Ž',
        '\u{8f}', '\u{90}', '‘', '’', '“', '”', '•', '–', '—', '˜', '™', 'š', '›', 'œ', '\u{9d}',
        'ž', 'Ÿ',
    ];
    bytes
        .iter()
        .map(|&b| match b {
            0x80..=0x9f => HIGH[(b - 0x80) as usize],
            _ => b as char,
        })
        .collect()
}
