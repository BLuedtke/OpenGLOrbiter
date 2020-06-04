//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.


#include "StandardModel.h"

StandardModel::StandardModel()
{
	//std::cout << "StandardModel Default Constructor" << std::endl;
}

StandardModel::~StandardModel()
{
	uShader = nullptr;
}

void StandardModel::setShader(std::unique_ptr<StandardShader> uPShader)
{
	uShader = std::move(uPShader);
}

void StandardModel::draw(const BaseCamera& Cam)
{
	if (uShader) {
		uShader->modelTransform(transform());
		uShader->activate(Cam);
	}
	else {
		std::cout << "StandardModel::draw() no shader found" << std::endl;
		return;
	}
}

const Matrix& StandardModel::transform()
{
	return this->uTransform;
}

void StandardModel::transform(Matrix pMatrix)
{
	this->uTransform = pMatrix;
}

