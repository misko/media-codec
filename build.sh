

sh bootstrap
./configure LDFLAGS="-Wl,--no-as-needed"
make
make install
cd vencoder
gcc -shared  -fPIC -DPIC  .libs/libcedar_vencoder_la-BitstreamManager.o .libs/libcedar_vencoder_la-EncAdapter.o .libs/libcedar_vencoder_la-FrameBufferManager.o .libs/libcedar_vencoder_la-venc_device.o .libs/libcedar_vencoder_la-vencoder.o   -Wl,-rpath -Wl,/root/petbot-firmware/media-codec/sunxi-cedarx/SOURCE/base/.libs -Wl,-rpath -Wl,/root/petbot-firmware/media-codec/sunxi-cedarx/SOURCE/common/.libs -Wl,-rpath -Wl,/usr/local/lib/full-package-name -Wl,--no-as-needed -ldl -lcedar_plugin_venc -lcedar_plugin_vd_others -L../common/ ../base/.libs/libcedar_base.so ../common/.libs/libcedar_common.so  -O2 -Wl,--no-undefined -Wl,--no-as-needed   -Wl,-soname -Wl,libcedar_vencoder.so -o .libs/libcedar_vencoder.so
cd ..
cp ./vencoder/.libs/libcedar_vencoder.so /usr/local/lib/full-package-name/
#cp ./SOURCE/common/libcedar_plugin_v* /usr/local/lib/full-package-name/
cp ./common/libcedar_plugin_v* /usr/local/lib/full-package-name/

echo /usr/local/lib/full-package-name/ > /etc/ld.so.conf.d/mediacodec 
ldconfig # or something here 

#cp ./common/*.so /usr/lib/
#cp /usr/local/lib/full-package-name/libcedar_* /usr/lib/

#be super careful!
#some systems do not link unused libraries, and libtool is broken to fix this
#https://sigquit.wordpress.com/2011/02/16/why-asneeded-doesnt-work-as-expected-for-your-libraries-on-your-autotools-project/
#http://lists.gnu.org/archive/html/bug-libtool/2009-12/msg00038.html
#./configure LDFLAGS="-Wl,--no-as-needed" does not always work
#so you might need to compile manually
# in vencoder add in "-Wl,--no-as-needed" before library vd_others on the command line to link properly
#FML
