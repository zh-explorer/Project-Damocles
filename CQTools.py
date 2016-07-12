#coding=utf-8
from aliyunsdkcore import client
from aliyunsdkgreen.request.v20160627 import ImageDetectionRequest
import json

def check(url):
	fp = open('C:\\module\\AccessSecret','r')
	secret = fp.read()
	fp.close()
	clt = client.AcsClient("Emrkmf7tktwuRsMf", secret,'cn-hangzhou')

	request = ImageDetectionRequest.ImageDetectionRequest()
	request.set_accept_format('json')
	request.set_Async('false')
	request.set_ImageUrl(json.dumps([url]))
	request.set_Scene(json.dumps(["porn","ocr"]))
	response = clt.do_action(request)

	result = json.loads(response)
	if "Success" == result["Code"]:
	    imageResults = result["ImageResults"]["ImageResult"]
	    for imageDetectResult in imageResults:
	    	pornResult = imageDetectResult["PornResult"]
	    	if pornResult["Label"] == 1:
	    		return 3600 * 24
	    	elif pornResult["Label"] == 2:
	    		time = int(3600 * 3 *pornResult["Rate"]) / 100
	    		return time - time%60
	return 0


def aliCheck(msg):
	path = "C:\\nameless\\Damocles\\data\\image\\"
	stone = "C:\\log\\image"
	#logger = open("C:\\log\\log.txt",'a',0)
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