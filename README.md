# i3blocks_battery

Displays the battery's charging status and % remaining as an i3blocks
blocklet. A left-click on the blocklet will display a message using
`libnotify` showing how much time is left until the battery is full
(when charging) or empty (when discharging). Right-clicking gives a GTK
dialog with extended information about the battery.

## Dependencies

* `pkg-config`
* `libnotify`
* `gtk+-3.0`

## Building

```
    make
```

To build the blocklet with debug symbols, you can run

```
    make debug
```

## Installing

By default, this will install to ~/.local/bin. This can be changed with
by specifying DESTDIR and PREFIX.

```
    make install
```

## License

This project is licensed under the MIT License. Please see the LICENSE
file for more information.
