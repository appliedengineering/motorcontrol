pyzmq
---

When using pyzmq, make SURE to use version 19.0.2 by using the install command `pip install pyzmq==19.0.2 -v --install-option=--enable-drafts` (you can also use a newer version if you can verify it works). For whatever reason, recent versions of pyzmq don't like building with draft methods.

If you're getting errors with methods not showing up in cython, it's due to an invalid brew installation. From what I can tell, the latest `zeromq.rb` script is also corrupt when dealing with draft methods. You can find `zeromq.rb` at `/usr/local/Homebrew/Library/Taps/homebrew/homebrew-core/Formula`.

So far, the only `zeromq.rb` script that has been tested to work is as follows:
```
class Zeromq < Formula
  desc "High-performance, asynchronous messaging library"
  homepage "https://zeromq.org/"
  url "https://github.com/zeromq/libzmq/releases/download/v4.3.3/zeromq-4.3.3.tar.gz"
  sha256 "9d9285db37ae942ed0780c016da87060497877af45094ff9e1a1ca736e3875a2"

  livecheck do
    url :head
    regex(/^v?(\d+(?:\.\d+)+)$/i)
  end

  bottle do
    cellar :any
    sha256 "5310a4204987850db552bb2746c48171a0faf51692c9f52debf10f85ac4db569" => :catalina
    sha256 "b67097dee2a42630e01978befe5c173f2a976c16fe4190873997776a584e4559" => :mojave
    sha256 "fee7be7f0a0762755037f3cd2ab1207dc9af43b0b75bf517d5f6f522a315a3bd" => :high_sierra
  end

  head do
    url "https://github.com/zeromq/libzmq.git"

    depends_on "autoconf" => :build
    depends_on "automake" => :build
    depends_on "libtool" => :build
  end

  depends_on "asciidoc" => :build
  depends_on "pkg-config" => [:build, :test]
  depends_on "xmlto" => :build

  def install
    # Work around "error: no member named 'signbit' in the global namespace"
    if MacOS.version == :high_sierra
      ENV.delete("HOMEBREW_SDKROOT")
      ENV.delete("SDKROOT")
    end

    ENV["XML_CATALOG_FILES"] = "#{etc}/xml/catalog"

    # Disable libunwind support due to pkg-config problem
    # https://github.com/Homebrew/homebrew-core/pull/35940#issuecomment-454177261

    system "./autogen.sh" if build.head?
    system "./configure", "--enable-drafts","--disable-dependency-tracking", "--prefix=#{prefix}"
    system "make"
    system "make", "install"
  end

  test do
    (testpath/"test.c").write <<~EOS
      #include <assert.h>
      #include <zmq.h>

      int main()
      {
        zmq_msg_t query;
        assert(0 == zmq_msg_init_size(&query, 1));
        return 0;
      }
    EOS
    system ENV.cc, "test.c", "-L#{lib}", "-lzmq", "-o", "test"
    system "./test"
    system "pkg-config", "libzmq", "--cflags"
    system "pkg-config", "libzmq", "--libs"
  end
end
```

NOTE: if you do change the zeromq.rb script, you will get errors like [this](https://github.com/Homebrew/brew/issues/10286). All you have to do is reset the forumulae by running this command: `brew update-reset $(brew --repo homebrew/core)`.
