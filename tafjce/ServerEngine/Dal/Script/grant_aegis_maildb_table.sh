#!/bin/sh
DB_PREFIX='db_aegismail_'
DB_IP='%'
DB_USER='root'
DB_PWD=''
for((i=0;i<10;i++))
do
  echo "grant all to  ${DB_PREFIX}$i.*"
  mysql -uroot -e "grant ALL on ${DB_PREFIX}$i.* to $DB_USER@'$DB_IP' identified by '$DB_PWD'" 
done
