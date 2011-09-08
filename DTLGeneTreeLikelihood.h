/*
 *  DTLGeneTreeLikelihood.h
 *  ReconcileDuplications.proj
 *
 *  Created by boussau on 28/07/11.
 *  Copyright 2011 UC Berkeley. All rights reserved.
 *
 */
#ifndef _DTLGENETREELIKELIHOOD_H_
#define _DTLGENETREELIKELIHOOD_H_

// From the STL:
#include <iostream>
#include <iomanip>
#include <algorithm>

// From SeqLib:
#include <Bpp/Seq/Alphabet/Alphabet.h>
#include <Bpp/Seq/Container/VectorSiteContainer.h>
#include <Bpp/Seq/SiteTools.h>
#include <Bpp/Seq/App/SequenceApplicationTools.h>

// From PhylLib:
#include <Bpp/Phyl/Tree.h>
#include <Bpp/Phyl/Likelihood/DiscreteRatesAcrossSitesTreeLikelihood.h>
#include <Bpp/Phyl/Likelihood/HomogeneousTreeLikelihood.h>
#include <Bpp/Phyl/Likelihood/DRHomogeneousTreeLikelihood.h>
//#include <Phyl/NNIHomogeneousTreeLikelihood.h>
#include <Bpp/Phyl/Likelihood/ClockTreeLikelihood.h>
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


//#include <Phyl/SAHomogeneousTreeLikelihood.h>
#include "ReconciliationTools.h"
#include "ReconciliationTreeLikelihood.h"
#include "SpeciesTreeExploration.h"
#include "SpeciesTreeLikelihood.h"
#include "GeneTreeAlgorithms.h"



//From the BOOST library 
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>

#include "DTL.h"

namespace bpp 
{


  class DTLGeneTreeLikelihood 
  {
  private:
  //Parameter list
  std::map<std::string, std::string> params_;
  //Species tree (used to compute DTL score)
  Species_tree * DTLScoringTree_;
  TreeTemplate<Node> * spTree_;
  //Method to compute the DT score:
  std::string scoringMethod_;
  //Gene tree
  TreeTemplate<Node> * geneTree_;
  TreeTemplate<Node> * unrootedGeneTree_;
  std::string strGeneTree_;
  vector <string> strGeneTrees_;
  //Objects useful to compute the sequence likelihood
  Alphabet * alphabet_ ;
  VectorSiteContainer * sites_;
  SubstitutionModel*    model_ ;
  DiscreteDistribution* rDist_ ;
  //std::map to store the link between sequence and species.
  std::map<std::string, std::string> seqSp_;
  //DTL rates (in the order D, T, L):
  double delta_;
  double tau_;
  double lambda_;
  //Whether we should stop tree search
  bool stop_;
  //Index of the tree under study, and index of the most likely tree
  int index_;
  int bestIndex_;  
  //logLk and best logLk
  double DTLlogL_;
  double bestDTLlogL_;
  double sequencelogL_;
  double bestSequencelogL_;
  //Whether we should rearrange the gene trees
  bool rearrange_;
  //Number of iterations of the search algorithm without improvement
  int numIterationsWithoutImprovement_;
  //How far can we regraft subtrees when doing a spr
  int sprLimit_;
  
  
  public:
  //Simple constructor
  DTLGeneTreeLikelihood(std::map<std::string, std::string> & params) :
  params_(params)
  {
  parseOptions();
  }
  
  //Copy constructor should be done. Ignore for now.
  /*  DTLGeneTreeLikelihood(const DTLGeneTreeLikelihood& gtl):
  geneTree_(gtl.geneTree_), index_(gtl.index_),
  bestIndex_(gtl.bestIndex_), logL_(gtl.logL_), 
  bestlogL_(gtl.bestlogL_), {
  }*/
  
  //= operator should be done. Ignore for now.
  /*  DTLGeneTreeLikelihood& operator=(const DTLGeneTreeLikelihood& gtl):
   geneTree_(gtl.geneTree_), index_(gtl.index_),
   bestIndex_(gtl.bestIndex_), logL_(gtl.logL_), 
   bestlogL_(gtl.bestlogL_), {
   }*/
  
  //Destructor
  virtual ~DTLGeneTreeLikelihood() 
  {	
    if (DTLScoringTree_)
      delete DTLScoringTree_;
    if (geneTree_)
      delete geneTree_;
    if (unrootedGeneTree_)
      delete unrootedGeneTree_;
  }
  
  //Clone function should be done. Ignore for now.
  //  DTLGeneTreeLikelihood* clone() const { return new DTLGeneTreeLikelihood(*this); }

  //Get the logL of the species tree
  double getValue() const throw (Exception) { return DTLlogL_ + sequencelogL_; }
  
  //Computes the DTL likelihood of the current unrootedGeneTree_.
  pair<double, string> computeDTLLikelihood ();
  
  //Computes the sequence likelihood of the current unrootedGeneTree_.
  double optimizeSequenceLikelihood ();
  
  //Initializes various fields in the species tree
  void initialize();
  
  //Does a ML search for the best gene tree using SPRs and NNIs
  void MLSearch();
  
  //Outputs the gene tree
  void printGeneTree();
  
  protected:
  //Computes the loglk of the gene tree
  void computeLogLikelihood();
  //Parses the options and builds the SpeciesTreeLikelihoodObject
  void parseOptions();
  
  
  };

}

#endif //_DTLGENETREELIKELIHOOD_H_

