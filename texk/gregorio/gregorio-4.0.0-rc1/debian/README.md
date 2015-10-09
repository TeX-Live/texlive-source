# Building Gregorio for Debian

This directory contains the files to build the deb packages for Gregorio. It requires a Debian-based Linux distribution (Debian, Ubuntu, Mint, etc.).

### Simple build

If you just want `.deb` packages for personal use, simply run

    ./build-deb.sh

This will create the deb files in `build/`.

### Using the PPA

The Gregorio Project has set up a custom Ubuntu repository where recent versions of Gregorio are uploaded: [https://launchpad.net/~gregorio-project/+archive/ubuntu/gregorio](https://launchpad.net/~gregorio-project/+archive/ubuntu/gregorio). The previous link contains information on how to use it.

#### Building package for the PPA

First, if you want to upload packages for the PPA, create a [Launchpad](https://launchpad.net/) account and ask a member of the [gregorio team](https://launchpad.net/~gregorio-project) to invite you.

To prepare packages for Launchpad you need some specific settings:

 * Launchpad automatically builds the binary packages, so only source packages has to be uploaded
 * it doesn't accept non-Ubuntu distribution, so you have to put an Ubuntu version in the source package
 * you need to sign the package; for this, you need to have a GPG key (`gpg --gen-key` shoud be enough), and have it uploaded on your launchpad account

Once you know what you're doing, run

    ./build-deb.sh --source --email=*youremail* --fullname=*yourfullname* --version=*ubuntuversion*

where *youremail* and *yourfullname* are the email and full name associated to your gpg key, and *ubuntuversion* is a valid Ubuntu version (e.g. `trusty`).

It should build the necessary packages in `build/`.

#### Uploading

To upload the packages, from Ubuntu run

    dput ppa:gregorio-project/gregorio build/gregorio*.changes

or from Debian, create a `.dput.cf` file in your home directory containing

    [ppa-gregorio]
    fqdn     = ppa.launchpad.net
    method   = ftp
    incoming = ~gregorio-project/gregorio
    login    = anonymous

and run

    dput ppa-gregorio build/gregorio*.changes
