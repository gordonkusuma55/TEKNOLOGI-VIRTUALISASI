/* *********************************************************************
 * Copyright (C) 2007-2015 VMware, Inc. All Rights Reserved. -- VMware Confidential
 * *********************************************************************/

/* This demonstrates how to open a virtual machine,
 * power it on, and power it off.
 *
 * This uses the VixJob_Wait function to block after starting each
 * asynchronous function. This effectively makes the asynchronous
 * functions synchronous, because VixJob_Wait will not return until the
 * asynchronous function has completed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "vix.h"


/*
 * Certain arguments differ when using VIX with VMware Server 2.0
 * and VMware Workstation.
 *
 * Comment out this definition to use this code with VMware Server 2.0.
 */
#define USE_WORKSTATION

#ifdef USE_WORKSTATION

#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define  HOSTNAME ""
#define  HOSTPORT 0
#define  USERNAME ""
#define  PASSWORD ""

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI   // Launches the VMware Workstaion UI
                                                     // when powering on the virtual machine.

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

#else    // USE_WORKSTATION

/*
 * For VMware Server 2.0
 */

#define CONNTYPE VIX_SERVICEPROVIDER_VMWARE_VI_SERVER

#define HOSTNAME "https://192.2.3.4:8333/sdk"
/*
 * NOTE: HOSTPORT is ignored, so the port should be specified as part
 * of the URL.
 */
#define HOSTPORT 0
#define USERNAME "root"
#define PASSWORD "hideme"

#define  VMPOWEROPTIONS VIX_VMPOWEROP_NORMAL

#define VMXPATH_INFO "where vmxpath is a datastore-relative path to the " \
                     ".vmx file for the virtual machine, such as "        \
                     "\"[standard] ubuntu/ubuntu.vmx\"."

#endif    // USE_WORKSTATION


/*
 * Global variables.
 */

static char *progName;


/*
 * Local functions.
 */

////////////////////////////////////////////////////////////////////////////////
static void
usage()
{
   fprintf(stderr, "Usage: %s <vmxpath>\n", progName);
   fprintf(stderr, "%s\n", VMXPATH_INFO);
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    VixError err;
    char *vmxPath;
    VixHandle hostHandle = VIX_INVALID_HANDLE;
    VixHandle jobHandle = VIX_INVALID_HANDLE;
    VixHandle vmHandle = VIX_INVALID_HANDLE;

    progName = argv[0];
	char *metode;
	
    if (argc > 1) {
		vmxPath = argv[1];
		metode = argv[2];
    } else {
        usage();
        exit(EXIT_FAILURE);
    }



    jobHandle = VixHost_Connect(VIX_API_VERSION,
                                CONNTYPE,
                                HOSTNAME, // *hostName,
                                HOSTPORT, // hostPort,
                                USERNAME, // *userName,
                                PASSWORD, // *password,
                                0, // options,
                                VIX_INVALID_HANDLE, // propertyListHandle,
                                NULL, // *callbackProc,
                                NULL); // *clientData);
    err = VixJob_Wait(jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &hostHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto abort;
    }

    Vix_ReleaseHandle(jobHandle);
    jobHandle = VixVM_Open(hostHandle,
                           vmxPath,
                           NULL, // VixEventProc *callbackProc,
                           NULL); // void *clientData);
    err = VixJob_Wait(jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &vmHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        goto abort;
	}

	if (metode[0] == 's' && metode[1] == 't' && metode[2] == 'a' && metode[3] == 'r' && metode[4] == 't'){
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_PowerOn(vmHandle,
			VMPOWEROPTIONS,
			VIX_INVALID_HANDLE,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
		}

	}
	else if (metode[0] == 's' && metode[1] == 't' && metode[2] == 'o' && metode[3] == 'p'){
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_PowerOff(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
		}

	}
	else if (metode[0] == 's' && metode[1] == 'u' && metode[2] == 's' && metode[3] == 'p' && metode[4] == 'e' && metode[5] == 'n' && metode[6] == 'd'){
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_Suspend(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
		}

	}
	else if (metode[0] == 'd' && metode[1] == 'e' && metode[2] == 'l' && metode[3] == 'e' && metode[4] == 't' && metode[5] == 'e'){
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_Delete(
			vmHandle,
			VIX_VMDELETE_DISK_FILES,
			NULL,
			NULL
			);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_OK != err) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);

	}
   

    

abort:
    Vix_ReleaseHandle(jobHandle);
    Vix_ReleaseHandle(vmHandle);

    VixHost_Disconnect(hostHandle);

    return 0;
}

