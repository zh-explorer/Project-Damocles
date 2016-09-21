#!/usr/bin/env python2
# coding: utf-8
# file: main.py
# time: 2016/7/19 22:08

import feedparser
import datetime

__author__ = "lightless"
__email__ = "root@lightless.me"


def drops_spider(date=None):
    target_url = 'http://drops.hduisa.cn/feed'
    feeds = feedparser.parse(target_url)

    ret_val = list()
    for article in feeds.entries:
        tmp_date = article.published[:article.published.find('+') - 1]
        dd = datetime.datetime.strptime(tmp_date, '%a, %d %b %Y %H:%M:%S')

        if date:
            today = date
        else:
            today = datetime.date.today()

        if datetime.datetime.combine(today, datetime.time(0, 0)) < dd:
            item = dict()
            item['title'] = article.title
            item['content'] = article.content[0]['value'].lstrip('<p>').rstrip('</p>')
            item['author'] = article.author
            item['time'] = dd.strftime("%Y-%m-%d %H:%M:%S")
            item['link'] = article.link
            ret_val.append(item)
    return ret_val

if __name__ == "__main__":
    # 参数为空时抓取当天的
    # 有参数时抓取指定天的文章，
    # 下面的调用会抓取昨天的文章。
    result = drops_spider(datetime.date.today() - datetime.timedelta(days=1))
    print result



