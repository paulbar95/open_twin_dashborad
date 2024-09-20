import React, { useState } from 'react';
import Modal from 'react-modal';
import ServiceDetailsModal from './ServiceDetailsModal';
import './LocalServiceModal.scss';

Modal.setAppElement('#root');

const LocalSessionServiceModal = ({ isOpen, onRequestClose, modalContent, fetchServiceDetails, serviceDetails }) => {
    const [detailsModalIsOpen, setDetailsModalIsOpen] = useState(false);

    const openDetailsModal = async (url) => {
        await fetchServiceDetails(url);
        setDetailsModalIsOpen(true);
    };

    const closeDetailsModal = () => {
        setDetailsModalIsOpen(false);
    };

    return (
        <>
            <Modal
                isOpen={isOpen}
                onRequestClose={onRequestClose}
                contentLabel="Local Session Service Details"
                overlayClassName="modal-overlay"
                className="modal-content"
            >
                <h2 className='modal-header'>Local Session Service Details</h2>
                <button className='modal-close-button' onClick={onRequestClose}>Close</button>
                {modalContent && (
                    <>
                        <div className="usage usage-info">
                            <p>Global CPU usage: {parseFloat(modalContent["Global.CPU.Load"]).toFixed(2)}%</p>
                            <p>Global Memory usage: {parseFloat(modalContent["Global.Memory.Load"]).toFixed(2)}%</p>
                            <p>Process CPU usage: {parseFloat(modalContent["Process.CPU.Load"]).toFixed(2)}%</p>
                            <p>Process Memory usage: {parseFloat(modalContent["Process.Memory.Load"]).toFixed(2)}%</p>
                            <h5>Session Services:</h5>
                        </div>
                        {modalContent["Services.Supported"] && (
                            <ul className='supported-services'>
                                <h3>Supported Services:</h3>
                                {modalContent["Services.Supported"].map((service, index) => (
                                    <li key={index}>
                                        {service}
                                    </li>
                                ))}
                            </ul>
                        )}
                        <ul className='session-list'>
                            <h3>Session List:</h3>
                            {modalContent["Session.List"] && modalContent["Session.List"].map((session, index) => (
                                <li key={index}>
                                    <h4>Session ID: {session["Session.ID"]}</h4>
                                    <p>No. of active Services: {session["Session.Services"].length}</p>
                                    <ul>

                                        {session["Session.Services"].map((service, idx) => (
                                            <li className='session-service' key={idx}>
                                                {service["Service.URL"]} - {service["Service.Type"]}
                                                <button className='service-details-button' onClick={() => openDetailsModal(service["Service.URL"])}>Get Service Details</button>
                                            </li>
                                        ))}
                                    </ul>
                                </li>
                            ))}
                        </ul>
                    </>
                )}
            </Modal>

            <ServiceDetailsModal
                isOpen={detailsModalIsOpen}
                onRequestClose={closeDetailsModal}
                serviceDetails={serviceDetails}
            />
        </>
    );
};

export default LocalSessionServiceModal;
