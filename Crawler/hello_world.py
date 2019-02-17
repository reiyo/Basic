#!/usr/bin/python

import csv
import json
import os
import re
import sys
import urllib
import urllib2

from random import randint
from re import findall
from time import sleep

MAX_PAGE_CNT = 4
MAX_TRY_CNT = 10

def get_photos(interest_name, folder):
    url_prefix = "http://test"
    url_suffix = "test"

    output_folder = os.path.join(folder, interest_name)

    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for page_id in range(MAX_PAGE_CNT):
        photo_group_url = url_prefix + str(page_id+1) + url_suffix
        counter = 0;
    
        while 1:
            if counter >= MAX_TRY_CNT:
                break
            try:
                url_response = urllib2.urlopen( photo_group_url )
                break
            except:
                counter += 1
                sleep(randint(2, 20))
                pass

        if counter >= MAX_TRY_CNT:
            continue

        content = url_response.read()
        content_json = json.loads(content)
        photos_json = content_json.get('test_photos', {})

        if not photos_json:
            break
    
        for one_photo in photos_json:
            photo_url = one_photo.get('test_url', {})

            if not photo_url:
                continue

            temp_index = photo_url.rfind("/")
            photo_name = photo_url[temp_index+1:]
            photo_path = os.path.join(output_folder, photo_name)

            try:
                urllib.urlretrieve(photo_url, photo_path)
            except Exception,e:
                print e
                sleep(randint(2, 20))
                continue

        sleep(randint(2, 18))

def main():
    if len(sys.argv) < 2:
        print "Usage: python hello_world.py input.dat"
        return

    input_stream = open(sys.argv[1], 'rb')
    input_reader = csv.reader(input_stream, delimiter=' ', quotechar='"')

    output_folder = "photos"

    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for row in input_reader:
        interest_name = row[0]
        test_folder = os.path.join(output_folder, interest_name)

        if os.path.exists(test_folder):
            print "Skip non-empty " + interest_name
            continue

        get_photos(interest_name, interest_quote, output_folder)
        sys.stdout.flush()
        sleep(randint(60, 180))

    input_stream.close()

if __name__ == '__main__':
    main()
