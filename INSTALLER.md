# DIY Nostr signing device (powered by <a href="https://www.arduino.cc/reference/en/libraries/ubitcoin/">uBitcoin</a>)
## Flash here https://nsd.lnbits.com

<img style="width:500px;" src="https://i.imgur.com/wlV5isa.png">

For use with [https://github.com/fiatjaf/horse](https://github.com/fiatjaf/horse). Chrome Extension [here](https://chrome.google.com/webstore/detail/horse/ogdjeglchjlenflecdcoonkngmmipcoe)

Join us <a href="https://t.me/nostr_protocol">t.me/nostr_protocol</a>, <a href="https://t.me/lnbits">t.me/lnbits</a>, <a href="https://t.me/makerbits">t.me/makerbits</a>

This very cheap off the shelf nostr signing device is designed to work with Lilygos <a href="https://www.aliexpress.com/item/33048962331.html">Tdisplay</a>, but you can easily make work with any ESP32.

Data is sent to/from the **Signing Device** over webdev Serial, not the most secure data transmission method, but x1000 better than storing the privatekey on a computer. Clients could support sending to the device directly, but using is Nos2X is far more convenient.
