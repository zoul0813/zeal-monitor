# Zeal Monitor

This is a simple monitor program for the Zeal 8-bit Computer, running Zeal OS.

## Commands

```
r ADDR         - dump memory starting at ADDR to ADDR+256
r ADDR RANGE   - dump memory starting at ADDR to ADDR+RANGE
w ADDR [bytes] - write [bytes] starting at ADDR
s ADDR RANGE   - write ADDR to ADDR+RANGE to a.out
s ADDR RANGE f - write ADDR to ADDR+RANGE to 'f'
l ADDR         - load a.out into ADDR
s ADDR f       - load 'f' into ADDR
```

## Examples

```> r 8000 f
8000: CA FE DE AD BE EF 00 EA 00 00 EA EA 00 00 EA EA

> w 8000 EA EA EA EA
> r 8000 f
8000: EA EA EA EA BE EF 00 EA 00 00 EA EA 00 00 EA EA
```

You can save a block of memory to "a.out" with `s 8000 f` and then read it back in with `l 8000`

## Building from source

Make sure that you have [ZDE](https://github.com/zoul0813/zeal-dev-environment) installed.

Then open a terminal, go to the source directory and type the following commands:

```shell
    $ cd {project}
    $ zde make
    $ zde emu
```
