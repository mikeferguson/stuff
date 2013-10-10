## Packaging Notes

Key commands:

        dch -v 0.1-1 'comment'          # stamp a changelog
        dpkg-source -b folder_version   # build a source deb from source + orig.tar.gz
        dpkg-buildpackage -us -uc (-S)  # build a package, -S for source (-us, -uc say unsigned)

### dh
dh is kinda awesome, and kinda evil at the same time. A 3-liner works for 80% of packages:

        #!/usr/bin/make -f
        %:
            dh %@

However, you can override any number of dh_steps, with the _override_dh_step_ in the rule file.
A few common ones:

        override_dh_auto_configure:
            ./configure --with-kitchen-sink

        override_dh_clean:
            dh_clean
            rm -rf other_stuff

See more at http://joeyh.name/blog/entry/debhelper_dh_overrides/

### quilt
Creating patches with quilt:

        sudo apt-get install quilt

If using apt-get source instead of dget, apply the existing patches:

        quilt push -a

In exploded source tree:

        quilt new name_of_patch
        quilt edit <file>           (as many times as you need)
        quilt refresh               (exports new patch)

Alternatively use _quilt add_ and then edit the file externally. To update a patch:

        quilt push my.patch
        <edit stuff>
        quilt refresh my.patch

To undo patches from source tree:

        quilt pop -a

Useful ENV variables:

        export QUILT_REFRESH_ARGS="-p ab --no-timestamps --no-index" # makes a cleaner patch

### apt
Easily upgrade a number of packages:

        sudo apt-get --only-upgrade install ros-hydro-*
