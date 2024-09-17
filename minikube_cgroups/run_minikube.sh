#!/usr/bin/env bash

# launch with --dind to run minikube inside a docker container

set -euxo pipefail

if [ "$EUID" -ne 0 ]; then
    SUDO="sudo"
else
    SUDO=""
fi

# configure docker to use host cgroupns
echo $'{\n    "default-cgroupns-mode":"host"\n}' | $SUDO tee /etc/docker/daemon.json > /dev/null
$SUDO systemctl restart docker

if [[ "${1:-}" = "--dind" ]]; then
    docker build -t dind_systemd .
    docker run -d --rm -it --privileged --name dind_systemd --cgroupns=host  dind_systemd
    docker exec -i dind_systemd bash /run_minikube.sh
else
    curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64 && \
    $SUDO install minikube-linux-amd64 /usr/local/bin/minikube && $SUDO ln -s $(which minikube) /usr/local/bin/kubectl && rm minikube-linux-amd64

    minikube start --force-systemd=true --force --driver=docker --docker-opt="default-cgroupns-mode=host"
    kubectl create deployment nginx --image=nginx
    sleep 5
    pod=$(kubectl get pods -o name)
    kubectl wait --for=condition=Ready $pod
    kubectl exec $pod -- cat /proc/1/cgroup
fi

minikube delete
