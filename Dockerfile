#
#

FROM registry.access.redhat.com/ubi8/ubi as builder


#FROM registry.ci.openshift.org/ocp/4.10:base
WORKDIR /build/
RUN yum install -y make gcc git kernel-{core,devel,modules} elfutils-libelf-devel
RUN git clone -b multi-module --single-branch https://github.com/chr15p/simple-kmod.git
WORKDIR simple-kmod
RUN make all


FROM registry.access.redhat.com/ubi8-minimal:latest

RUN microdnf -y install kmod && microdnf clean all

RUN mkdir -p /modules/
# Add and build kmods-via-containers
COPY --from=builder /build/simple-kmod/*.ko /modules/

COPY --from=builder /build/simple-kmod/load-module.sh /modules/load-module.sh

ENTRYPOINT ["/modules/load-module.sh", ${MODNAME}]
