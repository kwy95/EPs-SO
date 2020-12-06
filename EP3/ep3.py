import datetime
import json

# abcdefg hijkl mn ABC DEF G 0123 45 6789

def create_directory(name):
	return {
		'Nome' : name,
		'files' : [],
		'Tempo_criado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
		'Tempo_modificado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
		'Tempo_acessado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
		'Dir' : 0,
	}

class Sistema(object):
	"""docstring for Sistema"""
	def __init__(self, filename):
		self.filename = filename
		self.metadados = {}
		self.bitmap = {}
		self.fat = {}

def mount(file):
	try:
		print('arquivos existe')
		f = open(file)
		sistema = Sistema(file)
		sistema.metadados = json.load(f)
		f.close()
	except IOError:
		print("arquivo não existe, criando um")
		sistema = Sistema(file)
		sistema.metadados = create_directory('/')
		f = open(file, 'w+')
		json.dump(sistema.metadados, f)
		f.close()
	return sistema

def ls(metadados, path_dir):
	if path_dir == ' ':
		for file in metadados['files']:
			print(file['Nome'])
	else:
		for file in metadados['files']:
			if 'files' in file and file['Nome'] == path_dir.split('/')[0]: #é um diretorio
				if path_dir.split('/')[0] == path_dir:
					ls(file, ' ')
				else:
					ls(file , '/'.join(path_dir.split('/')[1,-1]))

def save_mount(sistema):
	f = open(sistema.filename, 'w')
	json.dump(sistema.metadados, f)
	f.close()

def create_file(origem):
	nome = origem.split('\\')[-1]
	file = {
	'data' : [],
	'Nome' : nome,
	'Tamanho' : 0,
	'Tempo_criado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	'Tempo_modificado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	'Tempo_acessado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	}

	with open(origem, 'r') as f:
		block = f.read(4096)
		file['data'].append(block)
		file['Tamanho'] += block.__sizeof__() -33
		while block:
			block = f.read(4096)
			file['data'].append(block)
			file['Tamanho'] += block.__sizeof__()
	return file

def save_file(file, metadados, destino):
	if destino == ' ':
		metadados['files'].append(file)
	else:
		for i in range(len(metadados['files'])):
			file = metadados['files'][i]
			if 'files' in file and file['Nome'] == destino.split('/')[0]: #é um diretorio
				if destino.split('/')[0] == destino:
					metadados['files'][i] = save_file(file,metadados, ' ')
				else:
					metadados['files'][i] = save_file(file ,metadados, '/'.join(destino.split('/')[1,-1]))
	return metadados

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
			print(sist.metadados)
			comando = line.split(' ')[0]
			if comando == 'sai':
				break
			if comando == 'mkdir':
				name =  line.split(' ')[1]
				sist.metadados['files'].append(create_directory(name))
				save_mount(sist)
			if comando == 'umount':
				save_mount(sist)
				sist = None
				mounted = False
			if comando == 'ls':
				if len(line.split(' ')) == 1:
					ls(sist.metadados, ' ')
				else:
					ls(sist.metadados, line.split(' ')[1])
			if comando == 'cp':
				origem = line.split(' ')[1]
				if len(line.split(' ')) == 2:
					destino = ' '
				else:
					destino = line.split(' ')[2]
				file = create_file(origem)
				sist.metadados = save_file(file,sist.metadados, destino)

