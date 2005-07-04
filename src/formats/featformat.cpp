/**********************************************************************
Copyright (C) 1998-2001 by OpenEye Scientific Software, Inc.
Some portions Copyright (C) 2001-2005 by Geoffrey R. Hutchison
Some portions Copyright (C) 2004 by Chris Morley
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/

#include "mol.h"
#include "obconversion.h"
#include "obmolecformat.h"

using namespace std;
namespace OpenBabel
{

class FEATFormat : public OBMoleculeFormat
{
public:
    //Register this format type ID
    FEATFormat()
    {
        OBConversion::RegisterFormat("feat",this);
    }

    virtual const char* Description() //required
    {
        return
            "Feature format\n \
            No comments yet\n";
    };

  virtual const char* SpecificationURL()
  {return "";}; //optional

    //Flags() can return be any the following combined by | or be omitted if none apply
    // NOTREADABLE  READONEONLY  NOTWRITABLE  WRITEONEONLY
    virtual unsigned int Flags()
    {
        return READONEONLY;
    };

    //*** This section identical for most OBMol conversions ***
    ////////////////////////////////////////////////////
    /// The "API" interface functions
    virtual bool ReadMolecule(OBBase* pOb, OBConversion* pConv);
    virtual bool WriteMolecule(OBBase* pOb, OBConversion* pConv);
};
//***

//Make an instance of the format class
FEATFormat theFEATFormat;

/////////////////////////////////////////////////////////////////
bool FEATFormat::ReadMolecule(OBBase* pOb, OBConversion* pConv)
{

    OBMol* pmol = dynamic_cast<OBMol*>(pOb);
    if(pmol==NULL)
        return false;

    //Define some references so we can use the old parameter names
    istream &ifs = *pConv->GetInStream();
    OBMol &mol = *pmol;
    mol.SetTitle( pConv->GetTitle()); //default title is the filename

    char buffer[BUFF_SIZE];
    int i,natoms;

    ifs.getline(buffer,BUFF_SIZE);
    sscanf(buffer,"%d",&natoms);

    mol.ReserveAtoms(natoms);
    mol.BeginModify();

    if (!ifs.getline(buffer,BUFF_SIZE))
        return(false);
    mol.SetTitle(buffer);

    double x,y,z;
    char type[20];
    OBAtom *atom;
    for (i = 0; i < natoms;i++)
    {
        if (!ifs.getline(buffer,BUFF_SIZE))
            return(false);
        sscanf(buffer,"%s %lf %lf %lf",
               type,
               &x,
               &y,
               &z);
        CleanAtomType(type);
        atom = mol.NewAtom();
        atom->SetVector(x,y,z);
        atom->SetAtomicNum(etab.GetAtomicNum(type));
    }

    mol.EndModify();
    return(true);
}

////////////////////////////////////////////////////////////////

bool FEATFormat::WriteMolecule(OBBase* pOb, OBConversion* pConv)
{
    OBMol* pmol = dynamic_cast<OBMol*>(pOb);
    if(pmol==NULL)
        return false;

    //Define some references so we can use the old parameter names
    ostream &ofs = *pConv->GetOutStream();
    OBMol &mol = *pmol;

    char buffer[BUFF_SIZE];

    ofs << mol.NumAtoms() << endl;
    ofs << mol.GetTitle() << endl;

    OBAtom *atom;
    vector<OBNodeBase*>::iterator i;
    for(atom = mol.BeginAtom(i);atom;atom = mol.NextAtom(i))
    {
        sprintf(buffer,"%-3s %8.5f  %8.5f  %8.5f ",
                etab.GetSymbol(atom->GetAtomicNum()),
                atom->x(),
                atom->y(),
                atom->z());
        ofs << buffer << endl;
    }

    return(true);
}

} //namespace OpenBabel
