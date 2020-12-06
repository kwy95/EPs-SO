from datetime import datetime
import json

# abcdefg hijkl mn ABC DEF G 0123 45 6789
NBLOCOS = 250


def create_directory(name):
	now = str(int(datetime.timestamp(datetime.now())))
	return {
		'Nome' : name,
		'tam_bytes' : 0 + len(name), #!! colocar o numero minimo pelo json + nome
		'tam_blocos' : 1,
		'Tempo_criado' : now,#.strftime('%d/%m/%Y %H:%M'),
		'Tempo_modificado' : now,#.strftime('%d/%m/%Y %H:%M'),
		'Tempo_acessado' : now,#.strftime('%d/%m/%Y %H:%M'),
		'loc' : 0,
		'dir' : 1,
		'files' : [],
	}

class Sistema(object):
	"""docstring for Sistema"""
	def __init__(self, filename):
		self.filename = filename
		# self.bitmap = bytearray(b'\xff'*32)
		# self.bitmap[-1] -= 63
		self.bitmap = ([1]*NBLOCOS).append([0]*6)
		self.fat = [-1] * NBLOCOS
		self.metadados = {}
		# for i in range(25600):
		# 	data = {}
		# 	data['Data'] = ''
		# 	data['Next'] = -1
		# 	self.fat.append(data)

def get_bitfat(sist, fp):
	fp.seek(0)
	bmstr = fp.read(32)
	fatstr = fp.read(NBLOCOS)
	for h in zip(bmstr):
		bits = bin(int(h, 16))[2:]
		for b in zip(bits):
			sist.bitmap[i] = b

def update_bitfat(sist : Sistema, fp):
	bmstr = hex(int(lst2str(sist.bitmap), 2))[2:]
	fatstr = ""
	for v in sist.fat:
		fatstr += str(v+1)

	fp.seek(0)
	fp.write(bmstr + fatstr)


def lst2str(ls):
	s = ""
	for ss in ls:
		s += str(ss)
	return s

def print_arquivos(sist):
	pass

def fill_block(sz):
	st = "abcdefg hijkl mn ABC DEF G 0123 45 6789" * (int(sz/39)+1)
	return st[:sz]

def mount(file):
	try:
		f = open(file)
		print('arquivos existe')
		sistema = Sistema(file)
		get_bitfat(sistema, f)

		data = json.load(f)# usar json.loads() pra usar string
		sistema.metadados = data['metadados']
		sistema.fat = data['fat']
		f.close()
	except IOError:
		print("arquivo não existe, criando um")
		sistema = Sistema(file)
		sistema.metadados = create_directory('/')
		save_mount(sistema)
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
			loc = file['loc']
			memoria = sist.fat[loc]
			while memoria['Next'] != 1:
				sist.fat[loc]['Data'] = ''
				sist.fat[loc]['Next'] = -1
				loc = memoria['Next']
				memoria = sist.fat[loc]
		else:
			new_files.append(file)
	sist.metadados['files'] = new_files


def save_mount(sistema):
	f = open(sistema.filename, 'w+')
	f.write(bin(int(sistema.bitmap.hex(), 16))[2:])
	f.seek(256)
	data = {'fat' : sistema.fat,
	'metadados' : sistema.metadados}
	json.dump(data, f, indent=None)
	f.close()

def find_free_space(sist):
	for i in range(len(sist.fat)):
		if sist.fat[i]['Data'] == '':
			return i

def create_file(origem, sist):
	nome = origem.split('/')[-1]
	now = str(int(datetime.timestamp(datetime.now())))
	file = {
		'Nome' : nome,
		'tam_bytes' : 0,
		'tam_blocos' : 0,
		'Tempo_criado' : now,
		'Tempo_modificado' : now,
		'Tempo_acessado' : now,
		'loc' : -1,
		'dir' : 0,
	}
	loc = []
	with open(origem, 'r') as f:
		block = f.read(4096)
		i = find_free_space(sist)
		file['loc'] = i
		sist.fat[i]['Data'] = block
		loc.append(i)
		while block:
			block = f.read(4096)
			i = find_free_space(sist)
			sist.fat[i]['Data'] = block
			loc.append(i)
		for i in range(0,len(loc)-1):
			sist.fat[i]['Next'] = loc[i+1]
	return file

def save_file(file, metadados, destino):
	if destino == ' ':
		metadados['files'].append(file)
	else:
		for i in range(len(metadados['files'])):
			file = metadados['files'][i]
			if 'files' in file and file['Nome'] == destino.split('/')[0]: #é um diretorio
				if destino.split('/')[0] == destino:
					save_file(file, metadados, ' ')
				else:
					save_file(file, metadados, '/'.join(destino.split('/')[1,-1]))


mounted = False
while 1:
	line = input('[ep3]: ')
	comando = line.split(' ')[0]
	if comando == 'mount':
		file = line.split(' ')[1]
		sist = mount(file)
		mounted = True
	elif comando == 'sai':
		break
	else:
		if mounted == False:
			print('Você tem que montar algum arquivo para outros comandos')
		else:
			print(sist.metadados)
			if comando == 'mkdir':
				path = line.split(' ')[1].split('/')
				name = path[-1]
				# print(path)
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
				save_file(file, sist.metadados, destino)
			if comando == 'rm':
				arquivo = line.split(' ')[1]
				rm(arquivo, sist)

