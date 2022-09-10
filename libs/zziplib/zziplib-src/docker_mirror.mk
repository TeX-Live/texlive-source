
docker_mirror_RAW = https://raw.githubusercontent.com/gdraheim/docker-mirror-packages-repo
docker_mirror_py = docker_mirror.py
docker_mirror_pyi = docker_mirror.pyi
copy:
	@ if test -d ../docker-mirror-packages-repo; then : \
	; cp -v ../docker-mirror-packages-repo/docker_mirror.py $(docker_mirror_py) \
	; cp -v ../docker-mirror-packages-repo/docker_mirror.pyi $(docker_mirror_pyi) \
	; elif test -d ../../docker/docker-mirror-packages-repo; then : \
	; cp -v ../../docker/docker-mirror-packages-repo/docker_mirror.py $(docker_mirror_py) \
	; cp -v ../../docker/docker-mirror-packages-repo/docker_mirror.pyi $(docker_mirror_pyi) \
	; else : \
	; echo curl $(docker_mirror_RAW)/master/docker_mirror.py \
	; curl $(docker_mirror_RAW)/master/docker_mirror.py  -s -o $(docker_mirror_py) \
	; curl $(docker_mirror_RAW)/master/docker_mirror.pyi -s -o $(docker_mirror_pyi) \
	; fi \
	; chmod 775 $(docker_mirror_py) \
	; grep --with-filename __version__ $(docker_mirror_py) | sed 's/py:/py* : /'


