# htmlencode

CLI encoder and decoder for HTML entities

## Description

A command-line tool for encoding and decoding HTML entities, similar to base64 or urlencode. Converts special characters like `<`, `>`, `&`, `"`, `'` to their HTML entity equivalents (`&lt;`, `&gt;`, `&amp;`, `&quot;`, `&#39;`) and vice versa.

## Build

### Autotools Build

```sh
./autogen.sh && ./configure && make
```

Then `sudo make install` to install to `/usr/local/bin`

### Direct Compilation

```sh
gcc -Wall -Wextra -pedantic -O2 -s -o htmlencode src/main.c
```

## Usage

Reads from standard input, writes to standard output.

### Options

* `-a` encode all characters (using numeric entities)
* `-b` do not automatically encode non printable (i.e. binary) characters
* `-c` specify a different set of special characters when encoding
* `-d` decode data
* `-l` encode input line by line
* `-n` do not output the trailing newline when encoding
* `-x` use hexadecimal entities (&#xHH;) instead of decimal (&#NNN;)
* `-h` display help and exit

### Examples

**Encode HTML:**
```sh
echo '<script>alert("XSS")</script>' | htmlencode
```
Output: `&lt;script&gt;alert(&quot;XSS&quot;)&lt;/script&gt;`

**Decode HTML:**
```sh
echo '&lt;script&gt;alert(&quot;XSS&quot;)&lt;/script&gt;' | htmlencode -d
```
Output: `<script>alert("XSS")</script>`

**Encode all characters as numeric entities:**
```sh
echo "Hello" | htmlencode -a
```
Output: `&#72;&#101;&#108;&#108;&#111;`

**Use hex entities:**
```sh
echo "<test>" | htmlencode -ax
```
Output: `&#x3C;&#x74;&#x65;&#x73;&#x74;&#x3E;`

## Features

- **Named entities**: Common characters (`<`, `>`, `&`, `"`, `'`) are encoded as named entities by default
- **Numeric entities**: Can encode any character as decimal (`&#NNN;`) or hex (`&#xHH;`) numeric entities
- **Decode support**: Full decoding of both named and numeric (decimal/hex) entities
- **Line mode**: Process input line by line with `-l`
- **Binary safe**: Automatically encodes non-printable characters (unless disabled with `-b`)

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

