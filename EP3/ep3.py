from datetime import datetime
import json

# abcdefg hijkl mn ABC DEF G 0123 45 6789
NBLOCOS = 250 # numero de blocos
SBLOCOS = 4000 # numero de caracteres por bloco


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
		self.bitmap = [1]*NBLOCOS + [0]*6
		self.fat = [-1] * NBLOCOS
		self.metadados = {}

def load_bitfat(sist, fp):
	fp.seek(0)
	bmstr = fp.read(64)
	fatstr = fp.read(2*NBLOCOS)
	i = 0
	for h in zip(bmstr):
		# print(h)
		bits = bin(int(h[0], 16))[2:]
		for b in zip(bits):
			sist.bitmap[i] = b[0]
			i += 1
	i = 0
	count = 0
	hh = ""
	for h in zip(fatstr):
		if count % 2 == 0:
			hh = h[0]
		else:
			hh += h[0]
			pos = int(hh, 16) - 1 # ajuste de offset
			sist.fat[i] = pos
			i += 1
		count += 1

def update_bitfat(sist : Sistema, fp):
	bmstr = hex(int(lst2str(sist.bitmap), 2))[2:]
	bmstr_l = '0' * (32 - len(bmstr)) + bmstr
	fatstr_l = ""
	for v in sist.fat:
		fatstr = hex(v+1)[2:] # offset de 1 para evitar numeros negativos
		fatstr_l += '0' * (2 - len(fatstr)) + fatstr
	fill = fill_block(SBLOCOS - 564)

	fp.seek(0)
	fp.write(bmstr_l + fatstr_l + fill)



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
		load_bitfat(sistema, f)
		f.seek(SBLOCOS)
		d = f.read()
		print(d)
		data = json.loads(d)# usar json.loads() pra usar string
		sistema.metadados = data['metadados']
		# sistema.fat = data['fat']
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
	update_bitfat(sistema, f)
	data = { 'metadados' : sistema.metadados }
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

