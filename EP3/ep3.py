from datetime import datetime
import json

# abcdefg hijkl mn ABC DEF G 0123 45 6789
NBLOCOS = 250 # numero de blocos
SBLOCOS = 4000 # numero de caracteres por bloco
DLIM = 25


class Sistema(object):
	"""docstring for Sistema"""
	def __init__(self, filename):
		self.filename = filename
		self.bitmap = [1]*NBLOCOS + [0]*6
		self.fat = [-1]*NBLOCOS
		self.root = None
		self.blocos = ['[]']*NBLOCOS

def get_obj(path, sist):
	cur = sist.root
	bl = cur['loc']
	# print(cur, bl)
	obj = json.loads(sist.blocos[bl])
	count = 0
	for nxt in path:
		count += 1
		if nxt == '':
			continue
		for o in obj:
			if o['Nome'] == nxt:
				cur = o
				if o['dir'] == 1 and count < len(path):
					bl = o['loc']
					obj = json.loads(sist.blocos[bl])
				break
	return cur, obj, bl


def mkdir(path, sist):
	parent, p_obj, p_bl = get_obj(path.split('/')[:-1], sist)
	location = find_free_space(sist)

	if parent['tam_by'] < 25 and location != -1:
		name = path.split('/')[-1]
		now = str(int(datetime.timestamp(datetime.now())))
		novo = {
			'Nome' : name,
			'tam_by' : 0,
			'tam_bl' : 1,
			't_0' : now,
			't_m' : now,
			't_a' : now,
			'loc' : location,
			'dir' : 1,
		}
		sist.bitmap[location] = 0
		sist.fat[location] = -1

		if parent['Nome'] == '/':
			sist.root['tam_by'] += 1
			sist.root['t_m'] = now
			sist.root['t_a'] = now
			update_bl0(sist)
		else:
			for o in p_obj:
				if o['Nome'] == parent['Nome']:
					o['tam_by'] += 1
					o['t_m'] = now
					o['t_a'] = now
					break
			sist.blocos[p_bl] = json.dumps(p_obj)

		parent_dir = json.loads(sist.blocos[parent['loc']])
		parent_dir.append(novo)

		sist.blocos[parent['loc']] = json.dumps(parent_dir)
	else:
		print("Nao ha espaco para criar o diretorio pedido", location, parent['tam_by'])

def rmdir(path, sist):
	parent, p_obj, p_bl = get_obj(path.split('/')[:-1], sist)
	name = path.split('/')[-1]
	for o in p_obj:
		if o['dir'] == 1 and o['Nome'] == name:
			_, new_p_obj, _ = get_obj(path, sist)
			for i in range(len(new_p_obj)):
				file = new_p_obj[i]
				rmdir(path+'/'+file['Nome'], sist)
			parent_dir = json.loads(sist.blocos[parent['loc']])
			new_data = []
			for data in parent_dir:
				if data['Nome'] == o['Nome']:
					pass
				else:
					new_data.append(data)
			parent_dir = new_data
			sist.blocos[parent['loc']] = json.dumps(parent_dir)
		if o['dir'] == 0 and o['name'] == name:
			rm(path+'/'+o['Nome'], sist)

def cp(origem, destino, sist):
	parent, p_obj, p_bl = get_obj(destino.split('/')[:-1], sist)
	location = find_free_space(sist)

	if parent['tam_by'] < 25 and location != -1:
		nome = destino.split('/')[-1]
		now = str(int(datetime.timestamp(datetime.now())))
		file = {
			'Nome' : nome,
			'tam_by' : 0,
			'tam_bl' : 1,
			't_0' : now,
			't_m' : now,
			't_a' : now,
			'loc' : -1,
			'dir' : 0,
		}
		count_bl = 0
		sz_bytes = 0
		with open(origem, 'r') as f:
			data = f.read(SBLOCOS)
			sz_bytes += len(data)
			count_bl += 1
			i = find_free_space(sist)
			file['loc'] = i
			# file['data'] = data
			sist.bitmap[i] = 0
			sist.fat[i] = -1
			sist.blocos[i] = data

			data = f.read(SBLOCOS)
			while data:
				j = find_free_space(sist)
				sz_bytes += len(data)
				count_bl += 1
				sist.blocos[j] = data
				sist.fat[i] = j
				sist.fat[j] = -1
				sist.bitmap[j] = 0
				data = f.read(SBLOCOS)
		file['tam_by'] = sz_bytes
		file['tam_bl'] = count_bl
		up = json.loads(sist.blocos[parent['loc']])
		up.append(file)
		sist.blocos[parent['loc']] = json.dumps(up)
	else:
		print('o arquivo nao cabe aqui')

def write_blocks(sist, i = -1):
	fp = open(sist.filename, "w+")
	data_str = ""
	if i != -1:
		data = sist.blocos[i]
		l = hex(len(data))[2:]
		lstr = '0'*(3-len(l)) + l
		data_str = lstr+ data + fill_block(SBLOCOS - len(data) - 3)
		fp.seek(i * SBLOCOS)
	else:
		for data in sist.blocos:
			l = hex(len(data))[2:]
			lstr = '0'*(3-len(l)) + l
			data_str += lstr + data + fill_block(SBLOCOS - len(data) - 3)
	fp.write(data_str)
	fp.close()

def load_sys(sist, fp):
	# fp = open(fn, 'r')
	fp.seek(0)
	sz = int(fp.read(3), 16)
	bmstr = fp.read(64)
	fatstr = fp.read(2*NBLOCOS)
	# print(bmstr, fatstr)
	i = 0
	for h in zip(bmstr):
		bt = bin(int(h[0], 16))[2:]
		bits = '0'*(4-len(bt)) + bt
		for b in zip(bits):
			# print(i, h, b, sz)
			sist.bitmap[i] = int(b[0])
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

	sist.root = json.loads(fp.read(sz - 64 - 2*NBLOCOS))

	fp.seek(0)
	sz = int(fp.read(3), 16)
	bloc = fp.read(sz)
	b = 0
	while b < NBLOCOS and bloc:
		sist.blocos[b] = bloc
		b += 1
		fp.seek(b * SBLOCOS)
		sz = int('0' + fp.read(3), 16)
		bloc = fp.read(sz)
	# fp.close()
	return sist

def update_bl0(sist : Sistema):
	bmstr = hex(int(lst2str(sist.bitmap), 2))[2:]
	bmstr_l = '0' * (64 - len(bmstr)) + bmstr
	fatstr_l = ""
	for v in sist.fat:
		fatstr = hex(v+1)[2:] # offset de 1 para evitar numeros negativos
		fatstr_l += '0' * (2 - len(fatstr)) + fatstr
	rtstr = json.dumps(sist.root)

	sist.blocos[0] = bmstr_l + fatstr_l + rtstr


def lst2str(ls):
	s = ""
	for ss in ls:
		s += str(ss)
	return s

def fill_block(sz):
	st = "abcdefg hijkl mn ABC DEF G 0123 45 6789" * (int(sz/39)+1)
	return st[:sz]

def mount(file):
	sistema = Sistema(file)
	try:
		f = open(file)
		# f.close()
		print('arquivos existe')
		sistema = load_sys(sistema, f)
		f.close()
	except IOError:
		print("arquivo não existe, criando um")
		now = str(int(datetime.timestamp(datetime.now())))
		sistema.root = {
			'Nome' : '/',
			'tam_by' : 0,
			'tam_bl' : 1,
			't_0' : now,
			't_m' : now,
			't_a' : now,
			'loc' : 1,
			'dir' : 1,
		}
		sistema.bitmap[0] = 0
		sistema.bitmap[1] = 0
		sistema.fat[0] = -1
		sistema.fat[1] = -1

		bmstr = hex(int(lst2str(sistema.bitmap), 2))[2:]
		bmstr_l = '0' * (32 - len(bmstr)) + bmstr
		fatstr_l = ""
		for v in sistema.fat:
			fatstr = hex(v+1)[2:] # offset de 1 para evitar numeros negativos
			fatstr_l += '0' * (2 - len(fatstr)) + fatstr
		sistema.blocos[0] = bmstr_l + fatstr_l + json.dumps(sistema.root)
		# sistema.blocos[1] = "[]"
		save_mount(sistema)
	return sistema

def ls(sist, path_dir):
	data, _, _ = get_obj(path_dir, sist)
	for file in json.loads(sist.blocos[data['loc']]):
		if file['dir'] == 0:
			print("nome:", file['Nome'], "; tamanho:", file['tam_by'], "; data de mod:", datetime.fromtimestamp(file['t_m']).strftime(%d/%m/%y - %H : %M))
		else:
			print("*nome:", file['Nome'], "; tamanho: ----; data de mod:", datetime.fromtimestamp(file['t_m']).strftime(%d/%m/%y - %H : %M))



def rm(arquivo, sist):
	new_files = []
	for file in sist.root['files']:
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
	sist.root['files'] = new_files

def cat(arq, sist):
	obj, p_obj, p_bl = get_obj(arq, sist)
	print(sist.blocos[obj['loc']])

def save_mount(sistema):
	# f = open(sistema.filename, 'w+')
	# update_bl0(sistema, f)
	write_blocks(sistema)
	# f.close()

def find_free_space(sist, start = 1):
	for i in range(start, NBLOCOS):
		if sist.bitmap[i] == 1:
			return i
	return -1


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
			if comando == 'mkdir':
				mkdir(line.split(' ')[1], sist)
			if comando == 'umount':
				save_mount(sist)
				sist = None
				mounted = False
			if comando == 'ls':
				ls(sist, line.split(' ')[1])
			if comando == 'cp':
				origem = line.split(' ')[1]
				destino = line.split(' ')[2]
				file = cp(origem, destino, sist)
				# save_file(file, sist.root, destino)
			if comando == 'rm':
				arquivo = line.split(' ')[1]
				rm(arquivo, sist)
			if comando == 'cat':
				arquivo = line.split(' ')[1]
				cat(arquivo, sist)
			# print(sist.blocos)


