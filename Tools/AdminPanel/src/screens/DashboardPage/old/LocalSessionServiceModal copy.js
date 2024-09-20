import React, { useState } from 'react';
import Modal from 'react-modal';
import ServiceDetailsModal from './ServiceDetailsModal';

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
            >
                <h2>Local Session Service Details</h2>
                <button onClick={onRequestClose}>Close</button>
                {modalContent && (
                    <>
                        <div className="usage">
                            <p>Global CPU usage: {modalContent["Global.CPU.Load"]}%</p>
                            <p>Global Memory usage: {modalContent["Global.Memory.Load"]}%</p>
                            <p>Process CPU usage: {modalContent["Process.CPU.Load"]}%</p>
                            <p>Process Memory usage: {modalContent["Process.Memory.Load"]}%</p>
                        </div>
                        <ul>
                            <h3>Session List:</h3>
                            {modalContent["Session.List"] && modalContent["Session.List"].map((session, index) => (
                                <li key={index}>
                                    <h4>Session ID: {session["Session.ID"]}</h4>
                                    <ul>
                                        <h5>Session Services:</h5>
                                        {session["Session.Services"].map((service, idx) => (
                                            <li key={idx}>
                                                {service["Service.URL"]} - {service["Service.Type"]}
                                                <button onClick={() => openDetailsModal(service["Service.URL"])}>Get Service Details</button>
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
