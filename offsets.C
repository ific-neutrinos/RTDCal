//sensor order in capsule
std::string sensor[4]={"1","2","3","0"};  // ref is the last
//std::string sensor[4]={"2","0","3","1"};  // ref is the last

//*************************************************************
void process_file(TFile* _file1, int index, int global_index,
                  TCanvas* c, TCanvas* c1,TCanvas* c2,
                  TH2F** h,TH1F** hm,TH1F** ha,
                  float dmax){
//*************************************************************

  //open ttree  
  TTree* temps = (TTree*)_file1->Get("temps");

  gStyle->SetTextSize(0.012);
  //  gStyle->SetTextColor(8);
  gStyle->SetOptStat(0);
  temps->SetLineColor(index+1);

  c1->SetGridy();
  c2->SetGridy();
  
  int color[3]={1,2,4};

  std::stringstream sdmax;
  sdmax<< dmax;
  
  TH1F* hh[3];
  TH1F* hhh[3];
  //-----------------------------------------------------------------------------  
  for (int j=0;j<3;j++){

    // Fill histos to compute mean of each set

    c1->cd();
    std::stringstream sss;
    sss << j;
    temps->Draw(("(temp["+sensor[j]+"]-temp["+sensor[3]+"])*1000>>hhm"+sss.str()).c_str(),("abs(temp["+sensor[j]+"]-temp["+sensor[3]+"])<"+sdmax.str()+" && time0>1000 && time0<1500").c_str());
    if (j==0) hhh[0] = (TH1F*)gPad->GetPrimitive(("hhm"+sss.str()).c_str());
    if (j==1) hhh[1] = (TH1F*)gPad->GetPrimitive(("hhm"+sss.str()).c_str());
    if (j==2) hhh[2] = (TH1F*)gPad->GetPrimitive(("hhm"+sss.str()).c_str());
    
    ha[j]->Fill(hhh[j]->GetMean());

    //------ Fill main plot --------------
    c2->cd();

    float m = hhh[j]->GetMean();
    float em = hhh[j]->GetRMS();
    hm[j]->SetBinContent(global_index,m);
    hm[j]->SetBinError(global_index,em);
    hm[j]->SetLineColor(color[j]);
    hm[j]->Draw("e1 same");

    //----- Fill offset vs time -------
    c->cd(j+1);  
    c->GetPad(j+1)->SetGridy();
    if (index==0){      
      temps->Draw(("(temp["+sensor[j]+"]-temp["+sensor[3]+"])>>hh"+sss.str()).c_str(),"","");//("abs(temp["+sensor[j]+"]-temp["+sensor[3]+"])<"+sdmax.str()).c_str());
      if (j==0) hh[0] = (TH1F*)gPad->GetPrimitive(("hh"+sss.str()).c_str());;
      if (j==1) hh[1] = (TH1F*)gPad->GetPrimitive(("hh"+sss.str()).c_str());
      if (j==2) hh[2] = (TH1F*)gPad->GetPrimitive(("hh"+sss.str()).c_str());
      
      //float a = hh[j]->GetMean();
      //h[j]->GetYaxis()->SetRangeUser(a-range,a+range);
      h[j]->Draw();
    } 
    
    temps->Draw((("temp["+sensor[j]+"]-temp["+sensor[3]+"]:time0")).c_str(),(("abs(temp["+sensor[j]+"]-temp["+sensor[3]+"])<"+sdmax.str())).c_str(),"l same");
  }  
  
  //-----Fill absolute temperature of ref sensor -----------
  c1->cd();
  if (index==0){
    temps->Draw((("temp["+sensor[3]+"]>>hh4")).c_str(),(("abs(temp[0]-temp["+sensor[3]+"])<"+sdmax.str())).c_str(),"l");
    float a = static_cast<TH1F*>(gPad->GetPrimitive("hh4"))->GetMean();
    h[3]->GetYaxis()->SetRangeUser(a-0.1,a+0.1);
    h[3]->Draw();
    } 
  temps->Draw((("temp["+sensor[3]+"]:time0")).c_str(),(("abs(temp[0]-temp["+sensor[3]+"])<"+sdmax.str())).c_str(),"l same");
}

//*************************************************************
void offsets(){
//*************************************************************

  //gROOT -> Reset();
  
  double dmin = -0.180;
  double dmax = 0.180;
  
  //the input filename list
  std::string listname= "/home/miguel/Documents/PhD/tmap/protoDUNE2/calibration/macros/tempCalibration/lists/offsets.list";

  const int NMAXFILES=500;
  TFile *file[NMAXFILES];
  
  //Open the file list
  FILE *pFile = fopen(listname.c_str(),"r");
 
  if(pFile == NULL) {
    std::cout << "Cannot open File list '" <<  listname << "'" << std::endl;
    exit(1);
  }

  //Count the number of files to be used in the list
  char file_name[200];
  int NFILES=0;
  while(fscanf(pFile,"%s",file_name) ==1 ){
    if((file_name[0] == '/' && file_name[1] == '/') || file_name[0] == '-') continue;
    NFILES++;
  }

  //close file and open it again
  fclose(pFile);
  pFile = fopen (listname.c_str(),"r");

  //create canvas and histograms
  TCanvas* c= new TCanvas("c","c",1400,400);
  c->Divide(3,1);

  TCanvas* c1= new TCanvas("c1","c1",800,500,500,500);
  TCanvas* c2= new TCanvas("c2","c2",0,000,1400,1000);

  std::stringstream sss;

  TH2F* h[4];
  TH1F* hm[3];
  TH1F* ha[100][3];
  TH1F* hm_all = new TH1F("hm_all","",NFILES,0,NFILES);
  TH1F* hb = new TH1F("hb","",50,0,50);
  TH1F* hc = new TH1F("hc","",50,0,50);
  hm_all->GetYaxis()->SetRangeUser(dmin*1000,dmax*1000);
  hm_all->SetXTitle("file number"); hm_all->SetYTitle("offsets (mk)"); 
  hc -> SetXTitle("Sensor RMS");

  c2->cd();
  hm_all->Draw();
  
  for(int j = 0; j < 3; j++){
    sss << j;
    hm[j] = new TH1F(("hm"+sss.str()).c_str(),("hm"+sss.str()).c_str(),NFILES,0,NFILES);
    h[j] = new TH2F(("h"+sss.str()).c_str(),("h"+sss.str()).c_str(),100,0,2500,100,dmin,dmax);
  }
  
  h[3] = new TH2F("h4","h4",100,0,2500,100,86.7,87.2);
  
  TLine*  lv[100];
  TLine*  lh[100][3];
  TText* tsensor[100][4];
  TText* tdate[100];
  std::string sensor[4];
  std::string date;

  int ifile=-1;
  int index=-1;
  int iset=0;

  //Horizontal lines to be drawn
  const int nlh_sensor = 3;
  std::string lh_sensor[nlh_sensor]={"37134","37130","39670"};
  
  //Reed sequenciallythe file names in the file list
  while(fscanf(pFile,"%s",file_name) == 1){

    // At the end of one set we find "---".
    // At that point create vertical and horizontal lines, add sensor numbers and mean values
    if (file_name[0] == '-' && file_name[1] == '-' && file_name[2] == '-'){
      c2->cd();

      // Create text with sensor number and average
      std::stringstream ss[3];
      std::stringstream ess[3];
      for(int j = 0; j < 3; j++){
        ss[j] << std::setprecision(3) << ha[iset][j]->GetMean();
        ess[j] << std::setprecision(2) << ha[iset][j]->GetRMS();        
        tsensor[iset][j] = new TText((ifile-index)*1.,ha[iset][j]->GetMean()+2,(sensor[j]+"="+ss[j].str()+"+-"+ess[j].str()).c_str());
        tsensor[iset][j]->Draw();
      }
      
      c2->cd();
      // reference sensor
      tsensor[iset][3] = new TText((ifile-index)*1.,(dmax*1.01)*1000,sensor[3].c_str());
      tsensor[iset][3]->Draw();

      tdate[iset] = new TText((ifile-index)*1.,(dmax*1.05)*1000,date.c_str());
      tdate[iset]->Draw();
      
      // Create Vertical line after one set
      lv[iset] = new TLine((ifile+1)*1.,dmin*1000,(ifile+1)*1.,dmax*1000);
      lv[iset]->SetLineStyle(3);
      lv[iset]->Draw();
      
      // Create Horizontal line at average of each sensor
      for (int j=0;j<3;j++){
        lh[iset][j] = new TLine(0,ha[iset][j]->GetMean(),NFILES,ha[iset][j]->GetMean());
        lh[iset][j]->SetLineStyle(3);

        // but only draw few of them
        for (int l=0;l<nlh_sensor;l++)          
          if (string(tsensor[iset][j]->GetTitle()).find(lh_sensor[l]) != std::string::npos)
            lh[iset][j]->Draw();
      }
      
      c->Update();
      c1->Update();
      c2->Update();
      
      iset++;
      gPad->WaitPrimitive();
      index = -1; 
      continue;

    }

    if (file_name[0] == '/' && file_name[1] == '/')continue;
    
    // open the root file
    TFile* file_temp = TFile::Open(file_name);
    if (!file_temp) continue;
        
    ifile++;
    index++;

    // Get the sensor numbers from the file
    std::string sfilename = string(file_name);
    std::string sfilename_rest = sfilename;
    
    for(int j = 0; j < 4; j++){
      sensor[j] = sfilename_rest.substr(sfilename_rest.find("_s")+2,5).c_str();
      sfilename_rest = sfilename_rest.substr(sfilename_rest.find("_s")+2,sfilename_rest.size()-1).c_str();
    }
    
    std::string sensor_ref=sensor[0];
    sensor[0]=sensor[1];
    sensor[1]=sensor[2];
    sensor[2]=sensor[3];
    sensor[3]=sensor_ref;

    date = sfilename.substr(sfilename.find("2020"),10).c_str();
    
    std::cout << ifile << " " << iset << " " << index << " " << file_name << " " << date << " "  << sensor[0] << " " << sensor[1] << " " << sensor[2] << std::endl;

    for (int j=0;j<3;j++){
      h[j]->SetTitle(sensor[j].c_str());
      h[j]->SetTitleOffset(1.4,"Y");
      h[j]->SetTitleSize(0.04,"X");
      h[j]->SetTitleSize(0.04,"Y");
      h[j]->SetXTitle("#it{t} (s)");
      //h[j]->GetXaxis()->SetRangeUser(1000,2000);;
      h[j]->SetYTitle("#it{Offset} (K)");
    }
    
    file[ifile] = file_temp;

    if (index==0){
      ha[iset][0] = new TH1F("ha1","ha1",1000,dmin*1000,dmax*1000);
      ha[iset][1] = new TH1F("ha2","ha2",1000,dmin*1000,dmax*1000);
      ha[iset][2] = new TH1F("ha3","ha3",1000,dmin*1000,dmax*1000);
    }
         
    // Process that file
    process_file(file[ifile],index,ifile+1,c,c1,c2,h,hm,ha[iset],dmax);

  }
  
}
