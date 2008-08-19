# $OpenBSD: Makefile.template,v 1.52 2007/06/27 07:15:32 steven Exp $
#

#
# Very short description of the package, 60 chars max
#
COMMENT=		"Statistical random number generator tests"

#
# What port/package will be created
#
DISTNAME=		dieharder-2.24.4

CATEGORIES=		math
SHARED_LIBS +=  dieharder	2.24      # .2.24

HOMEPAGE=		http://www.phy.duke.edu/~rgb/General/rand_rate.php

AUTOCONF_VERSION=	2.59
AUTOMAKE_VERSION=	1.9

# GPL
PERMIT_PACKAGE_CDROM=	Yes | "Reason if No"
PERMIT_PACKAGE_FTP=	Yes | "Reason if No"
PERMIT_DISTFILES_CDROM=Yes | "Reason if No"
PERMIT_DISTFILES_FTP=	Yes | "Reason if No"

WANTLIB=		c m

# where the source files and patches can be fetched
#
MASTER_SITES=		http://www.phy.duke.edu/~rgb/General/dieharder/

EXTRACT_SUFX=		.tgz

# Dependencies
#
BUILD_DEPENDS=          :autoconf-${AUTOCONF_VERSION}*:devel/autoconf/${AUTOCONF_VERSION}
LIB_DEPENDS=		gsl,gslcblas::devel/gsl

# build/configuration variables
#
SEPARATE_BUILD=		simple
USE_GMAKE=		Yes

# Programs that uses libtool should use this option,
# unless there is a really good reason not to.
USE_LIBTOOL=		Yes

CONFIGURE_STYLE=	automake

pre-configure:
	@cd ${WRKSRC} && AUTOCONF_VERSION=${AUTOCONF_VERSION} \
		AUTOMAKE_VERSION=${AUTOMAKE_VERSION} sh autogen.sh

.include <bsd.port.mk>
