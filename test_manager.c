
#include "orte.h"
#include "stdio.h"
#include "stdlib.h"
static ORTEDomain          *d;
int                        domain=1;//ORTE_DEFAULT_DOMAIN;
int                        regfail=0;
static ORTEDomainAppEvents events;
//event system
void
onRegFail(void *param) {
          printf("registration to a manager failed\n");
            regfail=1;
}
int main(int argc,char *argv[]) {
        ORTEInit();
        ORTEDomainInitEvents(&events);
        events.onRegFail=onRegFail;
        d=ORTEDomainMgrCreate(domain,NULL,&events,ORTE_TRUE);
        if (!d) {
                perror("ORTEDomainMgrCreate");
                exit(1);
        }
        ORTEDomainStart
        (d,ORTE_TRUE,ORTE_FALSE,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE);
        while(!regfail)
             ORTESleepMs(1000);
        ORTEDomainMgrDestroy(d);
        return 0;
}
