//===========================================
//  Lumina-DE source code
//  Copyright (c) 2015, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
// This function provides the basic current-user password validation
// The binary may need to have an effective root UID (setuid as root: "chmod 4555")
//   so that PAM can actually check the validity of the password.
//===========================================
//  SECURITY NOTE:
//    It is highly recomended that you have your PAM rules setup to disallow password checks for a time
//    after a number of failed attempts to prevent a user-level script from hammering this utility
//===========================================
//Standard C libary
#include <unistd.h>	// Standard C
#include <stdio.h> 	// Usage output
#include <pwd.h>  		// User DB information

//PAM/security libraries
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/openpam.h>

int main(int argc, char** argv){
  //Check the inputs
  if(argc!=2){
    //Invalid inputs - show the help text
    puts("lumina-checkpass: Simple user-level check for password validity (for screen unlockers and such).");
    puts("Usage: lumina-checkpass <password>");
    puts("Returns: 0 for a valid password, 1 for invalid");
    return 1;
  }
  //Validate current user (make sure current UID matches the logged-in user,
  char* cUser = getlogin();
  struct passwd *pwd = 0;
  pwd = getpwnam(cUser);
  if(pwd==0){ return 1; } //Login user could not be found in the database? (should never happen)
  if( getuid() != pwd->pw_uid ){ return 1; } //Current UID does not match currently logged-in user UID
  //Create the non-interactive PAM structures
  pam_handle_t *pamh;
  struct pam_conv pamc = { openpam_nullconv, NULL };
    //Place the user-supplied password into the structure
    int ret = pam_start( "system", cUser, &pamc, &pamh);
    if(ret != PAM_SUCCESS){ return 1; } //could not init PAM
    //char* cPassword = argv[1];
    ret = pam_set_item(pamh, PAM_AUTHTOK, argv[1]);
    //Authenticate with PAM
    ret = pam_authenticate(pamh,0); //this can be true without verifying password if pam_self.so is used in the auth procedures (common)
    if( ret == PAM_SUCCESS ){ ret = pam_acct_mgmt(pamh,0); } //Check for valid, unexpired account and verify access restrictions
    //Stop the PAM instance
    pam_end(pamh,ret);
  //return verification result
  return ((ret==PAM_SUCCESS) ? 0 : 1);
}
