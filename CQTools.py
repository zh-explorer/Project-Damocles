#coding=utf-8
from aliyunsdkcore import client
from aliyunsdkgreen.request.v20160627 import ImageDetectionRequest
import json
import news
import datetime

def check(url):

	fp = open('C:\\module\\AccessSecret','r')
	secret = fp.read()
	fp.close()
	clt = client.AcsClient("Emrkmf7tktwuRsMf", secret,'cn-hangzhou')

	request = ImageDetectionRequest.ImageDetectionRequest()
	request.set_accept_format('json')
	request.set_Async('false')
	request.set_ImageUrl(json.dumps([url]))
	request.set_Scene(json.dumps(["porn",]))
	response = clt.do_action(request)

	logger = open("C:\\log\\log.txt",'a',0)
	logger.write(response+'\n')
	logger.close()
	result = json.loads(response)

	if "Success" == result["Code"]:
		imageResults = result["ImageResults"]["ImageResult"]
		for imageDetectResult in imageResults:
			pornResult = imageDetectResult["PornResult"]
			if pornResult["Label"] == 1:
				return 3600 * 24
			elif pornResult["Label"] == 2:
				time = int(36 * 3 *pornResult["Rate"])
				return (time - time%60)+60
	return 0


def aliCheck(msg):
	path = "C:\\nameless\\Damocles\\data\\image\\"
	stone = "C:\\log\\image"
	time = 0
	namefile = open(stone,'r',0)
	existname = namefile.readlines()
	namefile.close()
	imageNames = getImageName(msg)
	for name in imageNames:
		if (name+'\n') in existname:
			continue

		imagePath = path + name + '.cqimg'
		image = open(imagePath)
		lines = image.readlines()
		image.close()
		for line in lines:
			if line[:4] == "url=":
				url = line[4:-1]
				break
		time = check(url)
		if time == 0:	
			namefile = open(stone,'a',0)
			namefile.write(name+'\n')
			namefile.close()
			existname.append(name)
		
	return time
def getImageName(msg):
	start = 0
	end = 0
	imageNames = []
	start = msg.find('[CQ:image,file=',end)
	while start != -1:
		end = msg.find(']',start)
		imageNames.append(msg[start+15:end])
		start = msg.find('[CQ:image,file=',end)
	return imageNames

def getNews():
	try:	
		ret = news.drops_spider(datetime.date.today() - datetime.timedelta(days=1))
		if len(ret) == 0:
			return "[CQ:at,qq=87294982]你又偷懒不更新drops了。快去更新".decode('utf8').encode('gbk')
		else:
			string = ""
			for i in ret:
				string += i['title'] + '\n'
				string += i['link']
				string += '\n\n'
			return string.encode('gbk')
	except Exception as e :
		fp = open('C:\\module\\error','w')
		p.write(e)
		fp.close()
		return "[CQ:at,qq=87294982]每日推送获取失败啦，快去修！".decode('utf8').encode('gbk')
def getDailyNews():
	import time
	t = time.localtime()
	if t.tm_hour == 12:
		return getNews()
	else: 
		return 'null'
	return "wtf"
