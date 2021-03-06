/*
Copyright or © or Copr. Centre National de la Recherche Scientifique
contributor : Bastien Boussau (2009-2013)

bastien.boussau@univ-lyon1.fr

This software is a bioinformatics computer program whose purpose is to
simultaneously build gene and species trees when gene families have
undergone duplications and losses. It can analyze thousands of gene
families in dozens of genomes simultaneously, and was presented in
an article in Genome Research. Trees and parameters are estimated
in the maximum likelihood framework, by maximizing theprobability
of alignments given the species tree, the gene trees and the parameters
of duplication and loss.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef _RECONCILIATIONTREELIKELIHOOD_H_
#define _RECONCILIATIONTREELIKELIHOOD_H_

#include <Bpp/Phyl/Likelihood/NNIHomogeneousTreeLikelihood.h>

// From NumCalc:
#include <Bpp/Numeric/VectorTools.h>
#include <Bpp/Numeric/Prob/DiscreteDistribution.h>
#include <Bpp/Numeric/Function/BrentOneDimension.h>
#include <Bpp/Numeric/Parametrizable.h>

#include "ReconciliationTools.h"
#include "GeneTreeAlgorithms.h"
#include "mpi.h" 

namespace bpp 
{

/**
 * @brief This class adds support for reconciliation to a species tree to the NNIHomogeneousTreeLikelihood class.
 */
class ReconciliationTreeLikelihood:
  public NNIHomogeneousTreeLikelihood
  {
    
    //  TreeTemplate<Node> * _tree;
    TreeTemplate<Node> * spTree_;
    TreeTemplate<Node> * rootedTree_;
    const std::map <std::string, std::string> seqSp_;
    std::map <std::string, int> spId_;
    std::vector <int> duplicationNumbers_;
    std::vector <int> lossNumbers_;
    std::vector <int>  branchNumbers_;

    std::vector <double> duplicationExpectedNumbers_;
    std::vector <double> lossExpectedNumbers_; 
    std::vector <int> num0Lineages_;
    std::vector <int> num1Lineages_;
    std::vector <int> num2Lineages_;
    std::set <int> nodesToTryInNNISearch_;
    double scenarioLikelihood_;
    mutable double _sequenceLikelihood;
    int MLindex_;
    bool rootOptimization_;
    mutable std::vector <int> tentativeDuplicationNumbers_;
    mutable std::vector <int> tentativeLossNumbers_; 
    mutable std::vector <int> tentativeBranchNumbers_; 
    mutable std::vector <int> tentativeNum0Lineages_;
    mutable std::vector <int> tentativeNum1Lineages_; 
    mutable std::vector <int> tentativeNum2Lineages_;
    mutable std::set <int> tentativeNodesToTryInNNISearch_;
    mutable int tentativeMLindex_;
    mutable double tentativeScenarioLikelihood_;
    mutable int totalIterations_;
    mutable int counter_;
    mutable std::vector <int> listOfPreviousRoots_;
    int _speciesIdLimitForRootPosition_;
    int heuristicsLevel_;
    mutable bool optimizeSequenceLikelihood_;
    mutable bool optimizeReconciliationLikelihood_;
    mutable bool considerSequenceLikelihood_;
    mutable bool DLStartingGeneTree_;
    unsigned int sprLimit_;

  public:
    /**
     * @brief Build a new ReconciliationTreeLikelihood object.
     *
     * @param tree The tree to use.
     * @param model The substitution model to use.
     * @param rDist The rate across sites distribution to use.
     * @param spTree The species tree
     * @param rootedTree rooted version of the gene tree
     * @param seqSp link between sequence and species names
     * @param spId link between species name and species ID
     * @param lossNumbers vector to store loss numbers per branch
     * @param lossProbabilities vector to store expected numbers of losses per branch
     * @param duplicationNumbers vector to store duplication numbers per branch
     * @param duplicationProbabilities vector to store expected numbers of duplications per branch
     * @param branchNumbers vector to store branch numbers in the tree
     * @param num0Lineages vectors to store numbers of branches ending with a loss
     * @param num1Lineages vectors to store numbers of branches ending with 1 gene
     * @param num2Lineages vectors to store numbers of branches ending with 2 genes
     * @param speciesIdLimitForRootPosition limit for gene tree rooting heuristics
     * @param heuristicsLevel type of heuristics used
     * @param MLindex ML rooting position
     * @param checkRooted Tell if we have to check for the tree to be unrooted.
     * If true, any rooted tree will be unrooted before likelihood computation.
     * @param verbose Should I display some info?
     * @throw Exception in an error occured.
     */
    ReconciliationTreeLikelihood(
                                 const Tree & tree,
                                 SubstitutionModel * model,
                                 DiscreteDistribution * rDist,
                                 TreeTemplate<Node> & spTree,  
                                 TreeTemplate<Node> & rootedTree, 
                                 const std::map <std::string, std::string> seqSp,
                                 std::map <std::string,int> spId,
                                 //std::vector <int> & lossNumbers, 
                                 std::vector <double> & lossProbabilities, 
                                 //std::vector <int> & duplicationNumbers, 
                                 std::vector <double> & duplicationProbabilities, 
                                 //std::vector <int> & branchNumbers,
                                 std::vector <int> & num0Lineages,
                                 std::vector <int> & num1Lineages,
                                 std::vector <int> & num2Lineages, 
                                 int speciesIdLimitForRootPosition,
                                 int heuristicsLevel,
                                 int & MLindex, 
                                 bool checkRooted = true,
                                 bool verbose = false,
                                 bool rootOptimization = false, 
                                 bool considerSequenceLikelihood = true, 
                                 bool DLStartingGeneTree = false, 
                                 unsigned int sprLimit = 2)
    throw (Exception);
    
    /**
     * @brief Build a new ReconciliationTreeLikelihood object.
     *
     * @param tree The tree to use.
     * @param data Sequences to use.
     * @param model The substitution model to use.
     * @param rDist The rate across sites distribution to use.
     * @param spTree The species tree
     * @param rootedTree rooted version of the gene tree
     * @param seqSp link between sequence and species names
     * @param spId link between species name and species ID
     * @param lossNumbers vector to store loss numbers per branch
     * @param lossProbabilities vector to store expected numbers of losses per branch
     * @param duplicationNumbers vector to store duplication numbers per branch
     * @param duplicationProbabilities vector to store expected numbers of duplications per branch
     * @param branchNumbers vector to store branch numbers in the tree
     * @param num0Lineages vectors to store numbers of branches ending with a loss
     * @param num1Lineages vectors to store numbers of branches ending with 1 gene
     * @param num2Lineages vectors to store numbers of branches ending with 2 genes
     * @param speciesIdLimitForRootPosition limit for gene tree rooting heuristics
     * @param heuristicsLevel type of heuristics used
     * @param MLindex ML rooting position     
     * @param checkRooted Tell if we have to check for the tree to be unrooted.
     * If true, any rooted tree will be unrooted before likelihood computation.
     * @param verbose Should I display some info?
     * @throw Exception in an error occured.
     */
    ReconciliationTreeLikelihood(
                                 const Tree & tree,
                                 const SiteContainer & data,
                                 SubstitutionModel * model,
                                 DiscreteDistribution * rDist,
                                 TreeTemplate<Node> & spTree,  
                                 TreeTemplate<Node> & rootedTree,  
                                 const std::map <std::string, std::string> seqSp,
                                 std::map <std::string,int> spId,
                                 //std::vector <int> & lossNumbers, 
                                 std::vector <double> & lossProbabilities, 
                                 //std::vector <int> & duplicationNumbers, 
                                 std::vector <double> & duplicationProbabilities, 
                                 //std::vector <int> & branchNumbers, 
                                 std::vector <int> & num0Lineages,
                                 std::vector <int> & num1Lineages,
                                 std::vector <int> & num2Lineages,  
                                 int speciesIdLimitForRootPosition,  
                                 int heuristicsLevel,
                                 int & MLindex, 
                                 bool checkRooted = true,
                                 bool verbose = false, 
                                 bool rootOptimization = false, 
                                 bool considerSequenceLikelihood = true, 
                                 bool DLStartingGeneTree = false, 
                                 unsigned int sprLimit = 2)
    throw (Exception);
    
    /**
     * @brief Copy constructor.
     */ 
    ReconciliationTreeLikelihood(const ReconciliationTreeLikelihood & lik);
    
    ReconciliationTreeLikelihood & operator=(const ReconciliationTreeLikelihood & lik);
    
    virtual ~ReconciliationTreeLikelihood();

    
      
#ifndef NO_VIRTUAL_COV
    ReconciliationTreeLikelihood*
#else
    Clonable*
#endif
    clone() const { return new ReconciliationTreeLikelihood(*this); }
    /*
     Copy all contents except alignment.
     */
      void copyContentsFrom  (const ReconciliationTreeLikelihood & lik) ;
      
    void initParameters();
    void resetMLindex() ;
    /**
     * @name The NNISearchable interface.
     *
     * Current implementation:
     * When testing a particular NNI, only the branch length of the parent node is optimized (and roughly).
     * All other parameters (substitution model, rate distribution and other branch length are kept at there current value.
     * When performing a NNI, only the topology change is performed.
     * This is up to the user to re-initialize the underlying likelihood data to match the new topology.
     * Usually, this is achieved by calling the topologyChangePerformed() method, which call the reInit() method of the LikelihoodData object.
     * @{
     */
    
    //double getLikelihood() const;
    
    double getLogLikelihood() const;
    
    void computeSequenceLikelihood();
    
    void computeReconciliationLikelihood();

    void computeTreeLikelihood();

    
    double getValue() const throw (Exception);

    void fireParameterChanged(const ParameterList & params);
    
    double getTopologyValue() const throw (Exception) { return getValue(); } 
    
    double getScenarioLikelihood() const throw (Exception) { return scenarioLikelihood_; }
    
    void setSpTree(TreeTemplate<Node> & spTree) { if (spTree_) delete spTree_; spTree_ = spTree.clone(); }
    
    void setSpId(std::map <std::string, int> & spId) {spId_ = spId;}
    
    double testNNI(int nodeId) const throw (NodeException);
    
    void doNNI(int nodeId) throw (NodeException);
    
    std::vector <int> getDuplicationNumbers();
    std::vector <int> getLossNumbers();
    std::vector <int> getBranchNumbers();
    std::vector <int> get0LineagesNumbers() const;
    std::vector <int> get1LineagesNumbers() const;
    std::vector <int> get2LineagesNumbers() const;
    
    TreeTemplate<Node> & getSpTree() const {return *spTree_;}
    
    TreeTemplate<Node> & getRootedTree() const {return *rootedTree_;}
    
    std::map <std::string, std::string> getSeqSp() {return seqSp_;}
    
    void setProbabilities(std::vector <double> duplicationProbabilities, std::vector <double> lossProbabilities);
    
    int getRootNodeindex();
    
    void resetSequenceLikelihood();
    
    double getSequenceLikelihood();
    
    void OptimizeSequenceLikelihood(bool yesOrNo) const  {
      optimizeSequenceLikelihood_ = yesOrNo;
    }

    void OptimizeReconciliationLikelihood(bool yesOrNo) const {
      optimizeReconciliationLikelihood_ = yesOrNo;
    }
  
    void print() const;
      
      /************************************************************************
       * Tries all SPRs at a distance < dist for all possible subtrees of the subtree starting in node nodeForSPR, 
       * and executes the ones with the highest likelihood. 
       ************************************************************************/
      void refineGeneTreeSPRs(map<string, string> params);
      
      

    
  };
  
    
} //end of namespace bpp.

#endif  //_RECONCILIATIONTREELIKELIHOOD_H_

