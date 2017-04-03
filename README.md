# Homebridge

![](https://media.giphy.com/media/10l79ICohTu4iQ/giphy.gif)

Homebridge is a lightweight NodeJS server you can run on your home network that emulates the iOS HomeKit API. It supports Plugins, which are community-contributed modules that provide a basic bridge from HomeKit to various 3rd-party APIs provided by manufacturers of "smart home" devices. 

Since Siri supports devices added through HomeKit, this means that with Homebridge you can ask Siri to control devices that don't have any support for HomeKit at all. For instance, using just some of the available plugins, you can say:

 * _Siri, unlock the back door._ [pictured above]
 * _Siri, open the garage door._
 * _Siri, turn on the coffee maker._ 
 * _Siri, turn on the living room lights._
 * _Siri, good morning!_

You can explore all available plugins at the NPM website by [searching for the keyword `homebridge-plugin`](https://www.npmjs.com/search?q=homebridge-plugin).

# Community

If you're having an issue with a particular plugin, open an issue in that plugin's Github repository. If you're having an issue with Homebridge itself, feel free to open issues and PRs here.

You can also chat with us in our nascent [Slack instance](http://homebridge-slackin.herokuapp.com).

# Installation

**Note:** If you're running on Linux, you'll need to make sure you have the `libavahi-compat-libdnssd-dev` package installed. If you're running on a Raspberry Pi, you should have a look at the [Wiki](https://github.com/nfarina/homebridge/wiki/Running-HomeBridge-on-a-Raspberry-Pi).

Homebridge is published through [NPM](https://www.npmjs.com/package/homebridge) and should be installed "globally" by typing:

    sudo npm install -g --unsafe-perm homebridge

You may need to use the `--unsafe-perm` flag if you receive an error similar to this:

    gyp WARN EACCES user "root" does not have permission to access the dev dir "/root/.node-gyp/5.5.0"

Now you should be able to run Homebridge:

    $ homebridge
    No plugins found. See the README for information on installing plugins.

Homebridge will complain if you don't have any Plugins installed, since it will essentially be useless, although you can still "pair" with it. See the next section "Installing Plugins" for more info.

Once you've installed a Plugin or two, you can run Homebridge again:

    $ homebridge
    Couldn't find a config.json file [snip]

However, Homebridge won't do anything until you've created a `config.json` file containing your accessories and/or platforms. You can start by copying and modifying the included `config-sample.json` file which includes declarations for some example accessories and platforms. Each Plugin will have its own expected configuration; the documentation for Plugins should give you some real-world examples for that plugin.

**NOTE**: Your `config.json` file MUST be inside of `.homebridge`, which is inside of your home folder.  On MacOS and Linux, the full path for your `config.json` would be `~/.homebridge/config.json`.  Any error messages will contain the exact path where your config is expected to be found.

**REALLY IMPORTANT**: You must use a "plain text" editor to create or modify `config.json`. Do NOT use apps like TextEdit on Mac or Wordpad on Windows.  Apps like these will corrupt the formatting of the file in hard-to-debug ways, making improper `"` signs is an example.  I suggest using the free [Atom text editor](http://atom.io).

Once you've added your config file, you should be able to run Homebridge again:

    $ homebridge
    Loaded plugin: homebridge-lockitron
    Registering accessory 'Lockitron'
    ---
    Loaded config.json with 1 accessories and 0 platforms.
    ---
    Loading 0 platforms...
    Loading 1 accessories...
    [Back Door] Initializing Lockitron accessory...

Homebridge is now ready to receive commands from iOS.

# Installing Plugins

Plugins are NodeJS modules published through NPM and tagged with the keyword `homebridge-plugin`. They must have a name with the prefix `homebridge-`, like **homebridge-mysmartlock**.

Plugins can publish Accessories and/or Platforms. Accessories are individual devices, like a smart switch or a garage door. Platforms act like a single device but can expose a set of devices, like a house full of smart lightbulbs.

You install Plugins the same way you installed Homebridge - as a global NPM module. For example:

    sudo npm install -g homebridge-lockitron

You can explore all available plugins at the NPM website by [searching for the keyword `homebridge-plugin`](https://www.npmjs.com/search?q=homebridge-plugin).

**IMPORTANT**: Many of the plugins that Homebridge used to include with its default installation have been moved to the single plugin [homebridge-legacy-plugins](https://www.npmjs.com/package/homebridge-legacy-plugins).

# Adding Homebridge to iOS

HomeKit itself is actually not an app; it's a "database" similar to HealthKit and PassKit. Where HealthKit has the companion _Health_ app and PassKit has _Passbook_, HomeKit has the _Home_ app, introduced with iOS 10.

If you are a member of the iOS developer program, you might also find Apple's [HomeKit Catalog](https://developer.apple.com/library/ios/samplecode/HomeKitCatalog/Introduction/Intro.html) app to be useful, as it provides straightforward and comprehensive management of all HomeKit database "objects".

Using the Home app (or most other HomeKit apps), you should be able to add the single accessory "Homebridge", assuming that you're still running Homebridge and you're on the same Wifi network. Adding this accessory will automatically add all accessories and platforms defined in `config.json`.

When you attempt to add Homebridge, it will ask for a "PIN code". The default code is `031-45-154` (but this can be changed, see `config-sample.json`).

Please See; https://github.com/nfarina/homebridge for more information.

# homebridge-mqtt

[![NPM version][npm-image]][npm-url]

[npm-image]: http://img.shields.io/npm/v/homebridge-mqtt.svg
[npm-url]: https://npmjs.org/package/homebridge-mqtt

Homebridge-mqtt is a Plugin for Homebridge. The design is based on MVC pattern, have a look at [homebridge-mvc](https://github.com/cflurin/homebridge-mvc). Homebridge-mqtt is a dynamic Plugin that allows you to add and control accessories from a "Bridge" or "Device" with a mqtt API. [Node-RED](http://nodered.org/) is the perfect platform to use with homebridge-mqtt.

Note-RED is a visual tool for wiring together hardware devices, APIs and online services.

### Installation

If you are new to Homebridge, please first read the Homebridge [documentation](https://www.npmjs.com/package/homebridge).
If you are running on a Raspberry, you will find a tutorial in the [homebridge-punt Wiki](https://github.com/cflurin/homebridge-punt/wiki/Running-Homebridge-on-a-Raspberry-Pi).

Install homebridge:
```sh
sudo npm install -g homebridge
```
Install homebridge-mqtt:
```sh
sudo npm install -g homebridge-mqtt
```

### Configuration
Add the mqtt-platform in config.json in your home directory inside `.homebridge`.

```sh
{
  "platform": "mqtt",
  "name": "mqtt",
  "url": "mqtt://127.0.0.1",
  "topic_prefix": "homebridge",
  "username": "foo",
  "password": "bar"
}

```

Replace `127.0.0.1` with the ip-address of your mqtt broker.

#
# mqtt API

The data (payload) is sent/received in a JSON format using following topics:


* homebridge/to/add
* homebridge/to/add/service
* homebridge/to/remove
* homebridge/to/remove/service
* homebridge/to/get
* homebridge/to/set
* homebridge/to/set/reachability
* homebridge/to/set/accessoryinformation
* homebridge/from/get
* homebridge/from/set
* homebridge/from/response
* homebridge/from/identify

#
**Version 0.3.0** and higher supports `multliple services`. To handle multiple services a new property `service_name` has been introduced.

**Note:** To add a service to an existing accessory (created prior version 0.3.0) please first remove the accessory and add it again.

Please see; https://github.com/cflurin/homebridge-mqtt for more information.