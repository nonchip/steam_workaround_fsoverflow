# description:

this is a work in progress workaround for issues like https://github.com/ValveSoftware/steam-for-linux/issues/4982

# building:

    cd build
    cmake ..
    make

# running:

    ./wrap.sh (optional arguments to steam)

# known issues:

it doesn't actually work yet, even though all the syscalls `[f]stat[v]fs[64]` are being wrapped and `wrap.sh` does the best I could to make it preload pretty much everywhere in steam, there are still a few calls that appear not to be wrapped correctly in strace and it doesn't show the modified values in either the "install game" or "manage libraries" dialogs.

wrapping tools like `df` actually produces the expected result though. I have no idea how steam still gets the values, probably by using some worker that doesn't get the same environment.

ways I try to prevent steam from kicking us out of `LD_PRELOAD`:

* wrap and filter `setenv`,`putenv`
* wrap and prevent `clearenv`
* wrap and prepend the paths to `execve`'s `envp[]` argument

still doesn't work though.

any input on this is **VERY** appreciated.
