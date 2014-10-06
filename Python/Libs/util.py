from json import loads,dumps

def split(value):
	MSB = value >> 8
	LSB = value & 255
	return [MSB,LSB]

def merge(MSB,LSB):
	return (MSB<<8 | LSB)

def readConfig(config_file = 'config.cfg'):
	inp = open(config_file,'r')
	input_conf = loads(inp.read())
	return input_conf

def setConfig(inp_conf,config_file = 'config.cfg'):
	out = open(config_file,'w')
	out.write(dumps(inp_conf))
	out.close()

def readAdress(adress_files = 'panels.cfg'):
	out = list()
	inp = open(adress_files,'r')
	for adress in xrange(int(inp.readline())):
		out.append(loads(inp.readline().strip())["Adress"])
	return out

def scale(setings,inp_dic):
	out = dict()
	mask = setings['Mask']

	#data['Azimuth'] = (payload['Azimuth'] * 0.441) - 2.54
	#data['Elevation'] = (payload['Elevation'] * 0.22) - 0.55

	out['Azimuth'] = inp_dic['Azimuth'] * (5.0/1023)
	out['Elevation'] = inp_dic['Elevation'] * (5.0/1023)

	for key in mask.keys():
		out[mask[key]] = inp_dic[key]

	out['Voltage'] = (out['Voltage'] * (5.0/1023))*(115.0/15)
	#out['Temperature'] = out['Temperature'] * (5.0/1023)
	out['Current'] = 73.3*(out['Current']*(5.0/1023)/5.0)-36.7
	out['ID'] = inp_dic['ID']

	return out