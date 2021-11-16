pyzmq + zeromq
---

https://zeromq.org/languages/python/

We're no longer using the unstable radio-dish communication pattern for ZeroMQ and we're migrating entirely to sub-pub. Therefore, we no longer need to custom compile our own version of the ZeroMQ library.

Ubuntu/Debian
---

```
sudo apt-get install libzmq3-dev
pip install pyzmq
```

MacOS
---

```
brew install zeromq
pip install pyzmq
```

