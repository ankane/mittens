require "mkmf"
require "open3"

vendor = File.expand_path("../../vendor/snowball", __dir__)
# CFLAGS from vendor/snowball/GNUmakefile and -fPIC
cflags = "-O2 -W -Wall -Wmissing-prototypes -Wmissing-declarations -fPIC"
output, status = Open3.capture2("make", "CFLAGS=#{cflags}", chdir: vendor)
puts output
raise "Command failed" unless status.success?

$INCFLAGS += " -I$(srcdir)/../../vendor/snowball/include"
$LDFLAGS += " $(srcdir)/../../vendor/snowball/libstemmer.a"

create_makefile("mittens/ext")
