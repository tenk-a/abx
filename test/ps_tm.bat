pushd data
powershell -Command Set-ItemProperty smp1 	-Name LastWriteTime -Value """2011-01-01 01:01:01"""
powershell -Command Set-ItemProperty smp2 	-Name LastWriteTime -Value """2012-02-02 02:02:02"""
powershell -Command Set-ItemProperty smp3 	-Name LastWriteTime -Value """2013-03-03 03:03:03"""
powershell -Command Set-ItemProperty smp4 	-Name LastWriteTime -Value """2014-04-04 04:04:04"""
powershell -Command Set-ItemProperty smp5 	-Name LastWriteTime -Value """2015-05-05 05:05:05"""
powershell -Command Set-ItemProperty smp6 	-Name LastWriteTime -Value """2016-06-06 06:06:06"""
powershell -Command Set-ItemProperty smp7 	-Name LastWriteTime -Value """2017-07-07 07:07:07"""
powershell -Command Set-ItemProperty smp1.smp 	-Name LastWriteTime -Value """2018-12-08 08:08:08"""
powershell -Command Set-ItemProperty smp2.smp 	-Name LastWriteTime -Value """2018-11-09 09:09:09"""
powershell -Command Set-ItemProperty smp3.smp 	-Name LastWriteTime -Value """2018-10-10 10:10:10"""
powershell -Command Set-ItemProperty smp4.smp 	-Name LastWriteTime -Value """2018-08-11 11:11:11"""
attrib -R smp5.smp
powershell -Command Set-ItemProperty smp5.smp 	-Name LastWriteTime -Value """2018-10-12 00:12:12"""

attrib /D +R smp2
attrib /D +H smp3
attrib /D +S smp4
attrib /D +R +H +S smp5

attrib +H smp4.smp
attrib +R smp5.smp

attrib -A +H smp_h.smp
attrib -A +I smp_i.smp
attrib -A +O smp_o.smp
attrib -A +R smp_r.smp
attrib -A +S smp_s.smp
attrib -A +X smp_x.smp
attrib -A +U smp_u.smp
attrib -A +P smp_p.smp

popd
