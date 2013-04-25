if [ -f /etc/man.conf ] && cmp -s /etc/defaults/etc/man.conf /etc/man.conf
then
    rm /etc/man.conf
fi

