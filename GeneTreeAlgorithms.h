/*
 *  GeneTreeAlgorithms.h
 *  ReconcileDuplications.proj
 *
 *  Created by boussau on 29/06/11.
 *  Copyright 2011 UC Berkeley. All rights reserved.
 *
 */

#ifndef _GENETREEALGORITHMS_H_
#define _GENETREEALGORITHMS_H_


// From SeqLib:
#include <Bpp/Seq/Alphabet/Alphabet.h>
#include <Bpp/Seq/Container/VectorSiteContainer.h>
#include <Bpp/Seq/SiteTools.h>
#include <Bpp/Seq/App/SequenceApplicationTools.h>

// From PhylLib:
#include <Bpp/Phyl/Tree.h>
#include <Bpp/Phyl/Likelihood.all>
#include <Bpp/Phyl/PatternTools.h>
#include <Bpp/Phyl/App/PhylogeneticsApplicationTools.h>
#include <Bpp/Phyl/Likelihood/MarginalAncestralStateReconstruction.h>
#include <Bpp/Phyl/OptimizationTools.h>
#include <Bpp/Phyl/Likelihood/RASTools.h>
#include <Bpp/Phyl/Io/Newick.h>
#include <Bpp/Phyl/Io/Nhx.h>
#include <Bpp/Phyl/TreeTools.h>
#include <Bpp/Phyl/Distance/BioNJ.h>
#include <Bpp/Phyl/OptimizationTools.h>
#include <Bpp/Phyl/Mapping.all>


// From NumCalc:
#include <Bpp/Numeric/Prob/DiscreteDistribution.h>
#include <Bpp/Numeric/Prob/ConstantDistribution.h>
#include <Bpp/Numeric/DataTable.h>
#include <Bpp/Numeric/Matrix/MatrixTools.h>
#include <Bpp/Numeric/VectorTools.h>
#include <Bpp/Numeric/AutoParameter.h>
#include <Bpp/Numeric/Random/RandomTools.h>
#include <Bpp/Numeric/NumConstants.h>
#include <Bpp/Numeric/Function/PowellMultiDimensions.h>

// From Utils:
#include <Bpp/Utils/AttributesTools.h>
#include <Bpp/App/ApplicationTools.h>
#include <Bpp/Io/FileTools.h>
#include <Bpp/Text/TextTools.h>
#include <Bpp/Clonable.h>
#include <Bpp/Numeric/Number.h>
#include <Bpp/BppString.h>
#include <Bpp/Text/KeyvalTools.h>




#include "ReconciliationTools.h"


/**************************************************************************
 * This function creates a sequence tree from a species tree and a std::map 
 * containing the link between the species and their sequences.
 **************************************************************************/
TreeTemplate<Node> * buildARandomSequenceTreeFromASpeciesTree (std::map <std::string, 
                                                               std::deque<std::string> > & spSeqs, 
                                                               TreeTemplate<Node> & tree, 
                                                               std::map <std::string, std::string> & spSelectedSeq);
/**************************************************************************
 * This function creates a sequence tree from a species tree and the std::map 
 * containing the link between the species and the putative orthologous sequence.
 **************************************************************************/
TreeTemplate<Node> * buildASequenceTreeFromASpeciesTreeAndCorrespondanceMap (TreeTemplate<Node> & tree, 
                                                                             std::map <std::string, 
                                                                             std::string> & spSelectedSeq);
/******************************************************************************/
// This function refines branch lengths of a gene tree.
/******************************************************************************/
double refineGeneTreeBranchLengthsUsingSequenceLikelihoodOnly (std::map<std::string, std::string> & params, 
                                                             TreeTemplate<Node>  *& unrootedGeneTree, 
                                                             VectorSiteContainer * sites, 
                                                             SubstitutionModel* model, 
                                                             DiscreteDistribution* rDist, 
                                                             string file, Alphabet *alphabet, bool mapping=false);
/******************************************************************************/
// This function maps substitutions in a gene tree.
/******************************************************************************/

vector< vector<unsigned int> > getCountsPerBranch(
                                                  DRTreeLikelihood& drtl,
                                                  const vector<int>& ids,
                                                  SubstitutionModel* model,
                                                  const SubstitutionRegister& reg,
                                                  SubstitutionCount *count,
                                                  bool stationarity = true,
                                                  double threshold = -1);
/******************************************************************************/
// This function optimizes branch lengths in a gene tree using substitution mapping
/******************************************************************************/

void optimizeBLMapping(
                       DRTreeLikelihood* tl,
                       double precision);
/******************************************************************************/
// This function builds a bionj tree
/******************************************************************************/
TreeTemplate<Node>  * buildBioNJTree (std::map<std::string, std::string> & params, 
                                      SiteContainer * sites, 
                                      SubstitutionModel* model, 
                                      DiscreteDistribution* rDist, 
                                      Alphabet *alphabet);
/******************************************************************************/
// This function refines a gene tree topology and branch lengths using the PhyML 
// algorithm.
/******************************************************************************/
void refineGeneTreeUsingSequenceLikelihoodOnly (std::map<std::string, std::string> & params, 
                                                TreeTemplate<Node>  *& unrootedGeneTree, 
                                                VectorSiteContainer * sites, 
                                                SubstitutionModel* model, 
                                                DiscreteDistribution* rDist, 
                                                string file, 
                                                Alphabet *alphabet);


/*
string parenthesisWithSpeciesNamesToGeneTree (TreeTemplate<Node> * geneTree,
                                              std::map<std::string, std::string > seqSp ) {  
  //,                                             std::vector<string> &geneNames) {
  
}
*/

/**************************************************************************
 * This function produces a string version of a gene tree, 
 * with gene names replaced by species names. 
 **************************************************************************/
string geneTreeToParenthesisWithSpeciesNames (TreeTemplate<Node> * geneTree,
                                              std::map<std::string, std::string > seqSp );

/**************************************************************************
 * This function produces a gene tree from a string version in which 
 * gene names have been changed to include species names. 
 **************************************************************************/
TreeTemplate<Node> * parenthesisPlusSpeciesNamesToGeneTree (string geneTreeStr) ;
  

/**************************************************************************
 * This function produces a string version of a gene tree, 
 * with gene names changed to include species names. 
 **************************************************************************/
string geneTreeToParenthesisPlusSpeciesNames (TreeTemplate<Node> * geneTree,
                                              std::map<std::string, std::string > seqSp );


/**************************************************************************
 * This function produces a gene tree with leaves annotated with species names.
 **************************************************************************/
void annotateGeneTreeWithSpeciesNames (TreeTemplate<Node> * geneTree,
                                       std::map<std::string, std::string > seqSp ) ;




#endif //_GENETREEALGORITHMS_H_