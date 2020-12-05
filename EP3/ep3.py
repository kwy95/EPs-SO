import datetime
import json

def create_directoy(name):
	return {
		'files' : [],
		'Nome' :name,
		'Tempo_criado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
		'Tempo_modificado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
		'Tempo_acessado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	}

class Sistema(object):
	"""docstring for Sistema"""
	def __init__(self, filename):
		self.filename = filename
		self.metadados = {}

def mount(file):
	try:
		f = open(file)
		sistema = Sistema(file)
		sistema.metadados = json.load(f)
		print('arquivos existe')
		f.close()
	except IOError:
		sistema = Sistema(file)
		sistema.metadados = create_directoy('/')
		f = open(file, 'w')
		json.dump(sistema.metadados, f)
		f.close()
		print("arquivo não existe, criando um")
	return sistema

def save_mount(sistema):
	f = open(sistema.filename, 'w')
	json.dump(sistema.metadados, f)
	f.close()

mounted = False
while 1:
	line = input('[ep3]: ')
	if line.split(' ')[0] == 'mount':
		file = line.split(' ')[1]
		sist = mount(file)
		mounted = True
	else:
		if mounted == False:
			print('Você tem que montar algum arquivo para outros comandos')
		else:
			comando = line.split(' ')[0]
			if comando == 'sai':
				break
			if comando == 'mkdir':
				name =  line.split(' ')[1]
				sist.metadados['files'].append(create_directoy(name))
				save_mount(sist)
			if comando == 'umount':
				save_mount(sist)
				sist = None
				mounted = False

