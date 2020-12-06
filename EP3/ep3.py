import datetime
import json

# abcdefg hijkl mn ABC DEF G 0123 45 6789

def create_directory(name):
	return {
		'Nome' : name,
		'files' : [],
		# 'Nome' :name,
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
		self.memory = []
		for i in range(25600):
			data = {}
			data['Data'] = ''
			data['Next'] = -1
			self.memory.append(data)

def mount(file):
	try:
		print('arquivos existe')
		f = open(file)
		sistema = Sistema(file)
		data = json.load(f)
		sistema.metadados = data['metadados']
		sistema.memory = data['memory']
		print('arquivos existe')
		f.close()
	except IOError:
		print("arquivo não existe, criando um")
		sistema = Sistema(file)
		sistema.metadados = create_directory('/')
		save_mount(sistema)
		print("arquivo não existe, criando um")
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


def rm(arquivo, sist):
	new_files = []
	for file in sist.metadados['files']:
		if file['Nome'] == arquivo:
			loc = file['localiz']
			memoria = sist.memory[loc]
			while memoria['Next'] != 1:
				sist.memory[loc]['Data'] = ''
				sist.memory[loc]['Next'] = -1
				loc = memoria['Next']
				memoria = sist.memory[loc]
		else:
			new_files.append(file)
	sist.metadados['files'] = new_files


def save_mount(sistema):
	f = open(sistema.filename, 'w')
	data = {'metadados' : sistema.metadados,
	'memory' : sistema.memory}
	json.dump(data, f)
	f.close()

def find_free_space(sist):
	for i in range(len(sist.memory)):
		if sist.memory[i]['Data'] == '':
			return i

def create_file(origem, sist):
	nome = origem.split('\\')[-1]
	file = {
	'localiz' : -1,
	'Nome' : nome,
	'Tamanho' : 0,
	'Tempo_criado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	'Tempo_modificado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	'Tempo_acessado' : datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
	}
	loc = []
	with open(origem, 'r') as f:
		block = f.read(4096)
		i = find_free_space(sist)
		file['localiz'] = i
		sist.memory[i]['Data'] = block
		loc.append(i)
		while block:
			block = f.read(4096)
			i = find_free_space(sist)
			sist.memory[i]['Data'] = block
			loc.append(i)
		for i in range(0,len(loc)-1):
			sist.memory[i]['Next'] = loc[i+1]
	return file

def save_file(file, metadados, destino):
	if destino == ' ':
		metadados['files'].append(file)
	else:
		for i in range(len(metadados['files'])):
			file = metadados['files'][i]
			if 'files' in file and file['Nome'] == destino.split('/')[0]: #é um diretorio
				if destino.split('/')[0] == destino:
					save_file(file,metadados, ' ')
				else:
					save_file(file ,metadados, '/'.join(destino.split('/')[1,-1]))


mounted = False
while 1:
	line = input('[ep3]: ')
	if line.split(' ')[0] == 'mount':
		file = line.split(' ')[1]
		sist = mount(file)
		mounted = True
	elif line.split(' ')[0] == 'sai':
		break
	else:
		if mounted == False:
			print('Você tem que montar algum arquivo para outros comandos')
		else:
			print(sist.metadados)
			comando = line.split(' ')[0]
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
				save_file(file,sist.metadados, destino)
			if comando == 'rm':
				arquivo = line.split(' ')[1]
				rm(arquivo, sist)

