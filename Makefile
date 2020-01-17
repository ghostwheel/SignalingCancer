# To make images yourself, run 'make images'
# To use images provided, run:
# make load-images
# once.
# Then do:
# to run programs: make run-main3 or make run-main2
# 
# To regenerate html files: make html
# 
.PHONY: images run-main3 run-main2 load-images html
html: main2.html main3.html Docker.html

load-images: 
	docker load -i docker.cancer-build.tgz
	docker load -i docker.lit-build.tgz
	docker load -i docker.cancer-release.tgz

images: docker.cancer-build.tgz docker.lit-build.tgz docker.cancer-release.tgz \
          main2.html main3.html Docker.html

docker.cancer-build.tgz: images.Dockerfile .dockerignore
	docker build --target builder -t mlachmann/cancer/build:0.1 -f images.Dockerfile .
	docker save mlachmann/cancer/build:0.1 | gzip > $@

docker.lit-build.tgz: images.Dockerfile .dockerignore
	docker build --target lit-builder -t mlachmann/lit/build:0.1 -f images.Dockerfile .
	docker save mlachmann/lit/build:0.1 | gzip > $@

docker.cancer-release.tgz: images.Dockerfile .dockerignore
	docker build --target release -t mlachmann/cancer/release:0.1 -f images.Dockerfile .
	docker save mlachmann/cancer/release:0.1 | gzip > $@

images.Dockerfile: Docker.md Literate/bin/lit
	lit -t Docker.md

.dockerignore: dot.dockerignore
	cp dot.dockerignore .dockerignore

main3.html: main3.md
	CONT=$$(docker create -t mlachmann/cancer/build:0.1 /bin/bash) ;\
	docker cp main3.md $$CONT:/tmp/ ;\
	docker start $$CONT ;\
	docker exec $$CONT lit main3.md ;\
	docker cp $$CONT:/tmp/main3.html main3.html ;\
	docker stop $$CONT ;\
	docker rm $$CONT

main2.html: main2.md
	CONT=$$(docker create -t mlachmann/cancer/build:0.1 /bin/bash) ;\
	docker cp main2.md $$CONT:/tmp/ ;\
	docker start $$CONT ;\
	docker exec $$CONT lit main2.md ;\
	docker cp $$CONT:/tmp/main2.html main2.html ;\
	docker stop $$CONT ;\
	docker rm $$CONT

Docker.html: Docker.md
	CONT=$$(docker create -t mlachmann/cancer/build:0.1 /bin/bash) ;\
	docker cp Docker.md $$CONT:/tmp/ ;\
	docker start $$CONT ;\
	docker exec $$CONT lit Docker.md ;\
	docker cp $$CONT:/tmp/Docker.html Docker.html ;\
	docker stop $$CONT ;\
	docker rm $$CONT

run-main3:
	docker run -it --rm mlachmann/cancer/release:0.1 main3

run-main2:
	docker run -it --rm mlachmann/cancer/release:0.1 main2

