Sample of using android's system library (libmedia.so) to record and play audio. It's faster than Java library and OpenSL. See image below to know how them work:

![alt tag](http://s4.postimg.org/8a5pkht8d/Screenshot_from_2015_03_20_00_17_13.png)

And we have two ways to use the libmedia.so:
  The fisrt is make a fake library what have the same function's name with libmedia.so. But the fake library will not implement any function. we just use it to reference libmedia.so:
    
![alt tag](http://s1.postimg.org/ve4i0gixb/Screenshot_from_2015_03_20_00_17_44.png)

  In the second, we use dlfcn library to open system library and call the function of system library. I use this way in my project because in this way, we can implement it for multi version of android. Because the libmedia.so can be update with the android's version. So, with same function we can have different implementation in different android's version:
  
![alt tag](http://s29.postimg.org/dtk8ui7uf/Screenshot_from_2015_03_20_00_17_46.png)

The image below will show you how to implement the second way:

![alt tag](http://s21.postimg.org/gppksynyf/Screenshot_from_2015_03_20_00_17_49.png)

Finally, this is the comparison of three way to implement record and play audio in android (Java library, OpenSL, Android system library):

![alt tag](http://s30.postimg.org/gr0t3kexd/Screenshot_from_2015_03_20_00_17_54.png)

