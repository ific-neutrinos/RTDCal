#include <algorithm>

const int NMAXFILES=500;

//***********************************************************
void process_tree(FILE* pFile,
		  const std::string& filename){
//***********************************************************
  //the input file name
  std::cout << "running on file " << filename << std::endl;
    
  //Create and open the output root file
  std::string rootfilename = filename+".root";
  TFile* file = new TFile(rootfilename.c_str(),"NEW");

  //Define the output tree
  char* cdate;
  double ftime;
  double ftime0;
  double ftemp[4];
  
  TTree* tree = new TTree("temps","temps");
  tree->Branch("date",  &cdate,  "date/C");
  tree->Branch("time",  &ftime,  "time/D");
  tree->Branch("time0", &ftime0, "time0/D");
  tree->Branch("temp",  &ftemp,  "temp[4]/D");

  tree->SetBranchAddress("date",  &cdate);           
  tree->SetBranchAddress("time",  &ftime);
  tree->SetBranchAddress("time0", &ftime0);
  tree->SetBranchAddress("temp",  ftemp);

  //Read the input file
  double ftime_ini = 0;
  char date[20],time[20],temp[4][20];
  int h, m, s = 0;
  while(fscanf(pFile,"%s%s%s%s%s%s",date,time,temp[0],temp[1],temp[2],temp[3]) == 6){
    //temps
    for(int i = 0 ; i < 4 ;i++){
      ftemp[i] = atof(temp[i]);
    }
    //Covert time into seconds
    if(sscanf(time,"%d:%d:%d",&h,&m,&s) >= 2){
      ftime = (double)(h *3600 + m*60 + s);
      if (ftime_ini==0) ftime_ini = ftime;
      ftime0 = ftime - ftime_ini;
    }
    
    cdate = date;
    tree->Fill();
  }

  tree->Write();
  file->Close();
  fclose(pFile); 
}

//***********************************************************
void create_trees(){
//***********************************************************
  
  //the input file name
  std::string listname= "lists/create_trees.list";
  
  FILE *file[NMAXFILES];
    
  //Open the flist file
  FILE *pFile = fopen (listname.c_str(),"r");
 
  if(pFile == NULL){
    std::cout << "Cannot open File '" <<  listname << "'" << std::endl;
    exit(1);
  }

  //read the list file
  char filename[200];
  int i = 0;
  //Read the input file
  while(fscanf(pFile,"%s",filename) == 1 && i < NMAXFILES){        
    file[i] = fopen (filename,"r");
    if(!file[i]){
      std::cout << "Cannot open File '" <<  filename << "'" << std::endl;
      continue;
    }
    process_tree(file[i],filename);
    i++;
  }

  //close listfile and end
  fclose(pFile);
}




