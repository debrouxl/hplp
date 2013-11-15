#! /bin/sh
#cd libhpcalcs/po; intltool-update --maintain; intltool-update --pot; intltool-update fr; cd ../..
cd libhpcalcs/po; make libhpcalcs.pot-update; make update-po; cd ../..
